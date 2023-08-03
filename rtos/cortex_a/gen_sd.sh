FW_NAME=sys_sec_rom_lnx_rfs_dtb_xtb.devfw

# only update B without bootloader
cp -f output.64/out/fwprog/devfw/${FW_NAME} nexar.bin

# only update A without bootloader
echo -n "FULL" > nexar_a.bin

let BLD_SIZE=`stat -c %s output.64/out/fwprog/devfw/bst_bld_pba.devfw.empty`
echo `printf "%.2X%.2X%.2X%.2X" $((BLD_SIZE & 0xff)) $((BLD_SIZE >> 8 & 0xff)) $((BLD_SIZE >> 16 & 0xff)) $((BLD_SIZE >> 24 & 0xff)) ` | xxd -r -ps >> nexar_a.bin

cat output.64/out/fwprog/devfw/bst_bld_pba.devfw.empty >> nexar_a.bin
cat output.64/out/fwprog/devfw/${FW_NAME} >> nexar_a.bin

# full update A with bootloader
echo -n "FULL" > nexar_full.bin

let BLD_SIZE=`stat -c %s output.64/out/fwprog/devfw/bst_bld_pba.devfw`
echo `printf "%.2X%.2X%.2X%.2X" $((BLD_SIZE & 0xff)) $((BLD_SIZE >> 8 & 0xff)) $((BLD_SIZE >> 16 & 0xff)) $((BLD_SIZE >> 24 & 0xff)) ` | xxd -r -ps >> nexar_full.bin

cat output.64/out/fwprog/devfw/bst_bld_pba.devfw >> nexar_full.bin
cat output.64/out/fwprog/devfw/${FW_NAME} >> nexar_full.bin
