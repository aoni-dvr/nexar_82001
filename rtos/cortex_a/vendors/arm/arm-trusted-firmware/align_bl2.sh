#!/bin/bash

echo "exec aglin_bl2.sh $1 $2"

#bl2.bin
FILE=$1
#bl2.nosig.bin
FILE_NOSIG="${FILE%.*}.nosig.bin"
#bl2.sig
FILE_SIG="${FILE%.*}.sig"

#round up bl2.bin size to multiple of 256 bytes
truncate --size %256 ${FILE}

#use id_rsa to sign bl2.bin, signature=bl2.sig
/usr/bin/openssl dgst -sha256 -sign $2 -out $FILE_SIG $1
cp $1 $FILE_NOSIG
cat $FILE_SIG >> $1
