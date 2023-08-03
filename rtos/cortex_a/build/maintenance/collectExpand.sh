#!/bin/bash

if [ "$1" != "" ]; then
    O=$1
else
    O=output
fi

# Script to strip .expand
cat > sed.script << EOF
s/funcdef_no/funcdef_no/; t;
s/(call (mem:SI (symbol_ref:SI (/(call (mem:SI (symbol_ref:SI (/; t;
D;

EOF

# Collect all .expand
(find $O -type f | grep expand$) > expand.txt
sed -i -e '\!app/fwupdater!d' expand.txt
sed -i -e '\!ssp/fwprog!d' expand.txt
sed -i -e '\!ssp/Bootup!d' expand.txt
sed -i -e '\!bsp.*/AmbaFlashFwProg_UserSysCtrl.c!d' expand.txt

rm -f all.expand
for f in `cat expand.txt`; do
    sed -i -f sed.script $f
    cat $f >> all.expand
done

rm -f sed.script
rm -f expand.txt

