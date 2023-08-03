#!/bin/bash -x

function pause() {
    read -s -n 1 -p "Press any key to continue . . ."
    echo ""
}

function bstini_parse() {
    cp $FILE_INI $PRESHMOO_DIR/test.ini
    cd $PRESHMOO_DIR
    $BSTINIPARSER test.ini
    cd -
    cp $PRESHMOO_DIR/test.h $FILE_H
    cp $PRESHMOO_DIR/test.ads $FILE_ADS
    cp $PRESHMOO_DIR/test.csf $FILE_CSF
}

# ========== CV2 ==========
function test_cv2() {
    PRESHMOO_DIR=~/DUMP_STORAGE_17/SDK8_BASE/rtos/cortex_a/tools/DramShmoo/PreShmoo/cv2x
    BSTINIPARSER=$PRESHMOO_DIR/bstiniparser_gen_all.sh

    FNAME=AmbaLPDDR4_MT53D512M32D2DS_840Mhz
    FILE_ASM_IN=test/cv2/$FNAME.asm
    FILE_INI=output/cv2_$FNAME.ini
    FILE_H=output/cv2_$FNAME.h
    FILE_ADS=output/cv2_$FNAME.ads
    FILE_CSF=output/cv2_$FNAME.csf
    FILE_ASM_OUT=output/cv2_$FNAME.asm
    FILE_DBG=output/cv2_$FNAME.dbg
    python3 rtos_dram_asm_converter.py -m 0 -p cv2 -o $FILE_INI $FILE_ASM_IN | tee $FILE_DBG
    bstini_parse
    python3 rtos_dram_asm_converter.py -m 1 -p cv2 -o $FILE_ASM_OUT $FILE_H | tee -a $FILE_DBG
    pause

    FNAME=AmbaLPDDR4_MT53D512M32D2DS_1800Mhz
    FILE_ASM_IN=test/cv2/$FNAME.asm
    FILE_INI=output/cv2_$FNAME.ini
    FILE_H=output/cv2_$FNAME.h
    FILE_ADS=output/cv2_$FNAME.ads
    FILE_CSF=output/cv2_$FNAME.csf
    FILE_ASM_OUT=output/cv2_$FNAME.asm
    FILE_DBG=output/cv2_$FNAME.dbg
    python3 rtos_dram_asm_converter.py -m 0 -p cv2 -o $FILE_INI $FILE_ASM_IN | tee $FILE_DBG
    bstini_parse
    python3 rtos_dram_asm_converter.py -m 1 -p cv2 -o $FILE_ASM_OUT $FILE_H | tee -a $FILE_DBG
    pause
}

# ========== CV22 ==========
function test_cv22() {
    PRESHMOO_DIR=~/DUMP_STORAGE_17/SDK8_BASE/rtos/cortex_a/tools/DramShmoo/PreShmoo/cv2x
    BSTINIPARSER=$PRESHMOO_DIR/bstiniparser_gen_all.sh

    FNAME=AmbaLPDDR4_MT53D512M32D2DS_840Mhz
    FILE_ASM_IN=test/cv22/$FNAME.asm
    FILE_INI=output/cv22_$FNAME.ini
    FILE_H=output/cv22_$FNAME.h
    FILE_ADS=output/cv22_$FNAME.ads
    FILE_CSF=output/cv22_$FNAME.csf
    FILE_ASM_OUT=output/cv22_$FNAME.asm
    FILE_DBG=output/cv22_$FNAME.dbg
    python3 rtos_dram_asm_converter.py -m 0 -p cv22 -o $FILE_INI $FILE_ASM_IN | tee $FILE_DBG
    bstini_parse
    python3 rtos_dram_asm_converter.py -m 1 -p cv22 -o $FILE_ASM_OUT $FILE_H | tee -a $FILE_DBG
    pause

    FNAME=AmbaLPDDR4_MT53D512M32D2DS_1596Mhz
    FILE_ASM_IN=test/cv22/$FNAME.asm
    FILE_INI=output/cv22_$FNAME.ini
    FILE_H=output/cv22_$FNAME.h
    FILE_ADS=output/cv22_$FNAME.ads
    FILE_CSF=output/cv22_$FNAME.csf
    FILE_ASM_OUT=output/cv22_$FNAME.asm
    FILE_DBG=output/cv22_$FNAME.dbg
    python3 rtos_dram_asm_converter.py -m 0 -p cv22 -o $FILE_INI $FILE_ASM_IN | tee $FILE_DBG
    bstini_parse
    python3 rtos_dram_asm_converter.py -m 1 -p cv22 -o $FILE_ASM_OUT $FILE_H | tee -a $FILE_DBG
    pause
}

# ========== CV25 ==========
function test_cv25() {
    PRESHMOO_DIR=~/DUMP_STORAGE_17/SDK8_BASE/rtos/cortex_a/tools/DramShmoo/PreShmoo/cv2x
    BSTINIPARSER=$PRESHMOO_DIR/bstiniparser_gen_all.sh

    FNAME=AmbaLPDDR4_MT53D512M32D2DS_840Mhz
    FILE_ASM_IN=test/cv25/$FNAME.asm
    FILE_INI=output/cv25_$FNAME.ini
    FILE_H=output/cv25_$FNAME.h
    FILE_ADS=output/cv25_$FNAME.ads
    FILE_CSF=output/cv25_$FNAME.csf
    FILE_ASM_OUT=output/cv25_$FNAME.asm
    FILE_DBG=output/cv25_$FNAME.dbg
    python3 rtos_dram_asm_converter.py -m 0 -p cv25 -o $FILE_INI $FILE_ASM_IN | tee $FILE_DBG
    bstini_parse
    python3 rtos_dram_asm_converter.py -m 1 -p cv25 -o $FILE_ASM_OUT $FILE_H | tee -a $FILE_DBG
    pause

    FNAME=AmbaLPDDR4_MT53D512M32D2DS_1596Mhz
    FILE_ASM_IN=test/cv25/$FNAME.asm
    FILE_INI=output/cv25_$FNAME.ini
    FILE_H=output/cv25_$FNAME.h
    FILE_ADS=output/cv25_$FNAME.ads
    FILE_CSF=output/cv25_$FNAME.csf
    FILE_ASM_OUT=output/cv25_$FNAME.asm
    FILE_DBG=output/cv25_$FNAME.dbg
    python3 rtos_dram_asm_converter.py -m 0 -p cv25 -o $FILE_INI $FILE_ASM_IN | tee $FILE_DBG
    bstini_parse
    python3 rtos_dram_asm_converter.py -m 1 -p cv25 -o $FILE_ASM_OUT $FILE_H | tee -a $FILE_DBG
    pause
}

# ========== CV28 ==========
function test_cv28() {
    PRESHMOO_DIR=~/DUMP_STORAGE_17/SDK8_BASE/rtos/cortex_a/tools/DramShmoo/PreShmoo/cv2x
    BSTINIPARSER=$PRESHMOO_DIR/bstiniparser_gen_all.sh

    FNAME=AmbaLPDDR4_MT53D512M16D1DS_1200Mhz
    FILE_ASM_IN=test/cv28/$FNAME.asm
    FILE_INI=output/cv28_$FNAME.ini
    FILE_H=output/cv28_$FNAME.h
    FILE_ADS=output/cv28_$FNAME.ads
    FILE_CSF=output/cv28_$FNAME.csf
    FILE_ASM_OUT=output/cv28_$FNAME.asm
    FILE_DBG=output/cv28_$FNAME.dbg
    python3 rtos_dram_asm_converter.py -m 0 -p cv28 -o $FILE_INI $FILE_ASM_IN | tee $FILE_DBG
    bstini_parse
    python3 rtos_dram_asm_converter.py -m 1 -p cv28 -o $FILE_ASM_OUT $FILE_H | tee -a $FILE_DBG
    pause

    FNAME=AmbaLPDDR4_MT53D512M16D1DS_1800Mhz
    FILE_ASM_IN=test/cv28/$FNAME.asm
    FILE_INI=output/cv28_$FNAME.ini
    FILE_H=output/cv28_$FNAME.h
    FILE_ADS=output/cv28_$FNAME.ads
    FILE_CSF=output/cv28_$FNAME.csf
    FILE_ASM_OUT=output/cv28_$FNAME.asm
    FILE_DBG=output/cv28_$FNAME.dbg
    python3 rtos_dram_asm_converter.py -m 0 -p cv28 -o $FILE_INI $FILE_ASM_IN | tee $FILE_DBG
    bstini_parse
    python3 rtos_dram_asm_converter.py -m 1 -p cv28 -o $FILE_ASM_OUT $FILE_H | tee -a $FILE_DBG
    pause
}

# ========== CV5 ==========
function test_cv5() {
    PRESHMOO_DIR=~/DUMP_STORAGE_17/SDK8_BASE/rtos/cortex_a/tools/DramShmoo/PreShmoo/cv5x
    BSTINIPARSER=$PRESHMOO_DIR/bstiniparser_gen_all.sh

    FNAME=AmbaLPDDR4_MT53E1G32D2FW_2112Mhz
    FILE_ASM_IN=test/cv5x/$FNAME.asm
    FILE_INI=output/cv5x_$FNAME.ini
    FILE_H=output/cv5x_$FNAME.h
    FILE_ADS=output/cv5x_$FNAME.ads
    FILE_CSF=output/cv5x_$FNAME.csf
    FILE_ASM_OUT=output/cv5x_$FNAME.asm
    FILE_DBG=output/cv5x_$FNAME.dbg
    python3 rtos_dram_asm_converter.py -m 0 -p cv5 -o $FILE_INI $FILE_ASM_IN | tee $FILE_DBG
    bstini_parse
    python3 rtos_dram_asm_converter.py -m 1 -p cv5 -o $FILE_ASM_OUT $FILE_H | tee -a $FILE_DBG
    pause

    FNAME=AmbaLPDDR5_1200Mhz
    FILE_ASM_IN=test/cv5x/$FNAME.asm
    FILE_INI=output/cv5x_$FNAME.ini
    FILE_H=output/cv5x_$FNAME.h
    FILE_ADS=output/cv5x_$FNAME.ads
    FILE_CSF=output/cv5x_$FNAME.csf
    FILE_ASM_OUT=output/cv5x_$FNAME.asm
    FILE_DBG=output/cv5x_$FNAME.dbg
    python3 rtos_dram_asm_converter.py -m 0 -p cv5 -o $FILE_INI $FILE_ASM_IN | tee $FILE_DBG
    bstini_parse
    python3 rtos_dram_asm_converter.py -m 1 -p cv5 -o $FILE_ASM_OUT $FILE_H | tee -a $FILE_DBG
    pause

    FNAME=AmbaLPDDR5_2496Mhz
    FILE_ASM_IN=test/cv5x/$FNAME.asm
    FILE_INI=output/cv5x_$FNAME.ini
    FILE_H=output/cv5x_$FNAME.h
    FILE_ADS=output/cv5x_$FNAME.ads
    FILE_CSF=output/cv5x_$FNAME.csf
    FILE_ASM_OUT=output/cv5x_$FNAME.asm
    FILE_DBG=output/cv5x_$FNAME.dbg
    python3 rtos_dram_asm_converter.py -m 0 -p cv5 -o $FILE_INI $FILE_ASM_IN | tee $FILE_DBG
    bstini_parse
    python3 rtos_dram_asm_converter.py -m 1 -p cv5 -o $FILE_ASM_OUT $FILE_H | tee -a $FILE_DBG
    pause
}

PS3="Select test target: "

select target in cv2 cv22 cv25 cv28 cv5 all exit
do
    case $REPLY in
        1)
            echo "$target"
            test_cv2
            break
            ;;
        2)
            echo "$target"
            test_cv22
            break
            ;;
        3)
            echo "$target"
            test_cv25
            break
            ;;
        4)
            echo "$target"
            test_cv28
            break
            ;;
        5)
            test_cv5
            break
            ;;
        6)
            test_cv2
            test_cv22
            test_cv25
            test_cv28
            test_cv5
            break
            ;;
        7)
            break
            ;;
        *)
            echo "Invalid option"
            ;;
    esac
done

