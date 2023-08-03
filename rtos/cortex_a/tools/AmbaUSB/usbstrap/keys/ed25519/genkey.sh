#!/bin/bash

PUBKEY_FILE=ed25519_pubkey.S
PUBKEY_BIN=ed25519_pubkey.bin

#openssl genpkey -algorithm Ed25519 -out priv.pem
#openssl pkey -in priv.pem -pubout -out pub.pem -text_pub
openssl pkeyutl -sign -inkey priv.pem -out sig.dat -rawin -in /bin/ls

# test
openssl pkeyutl -verify -pubin -inkey pub.pem -rawin -in /bin/ls -sigfile sig.dat; rm -rf sig.dat

# genpkey for simulation
echo ".globl ed25519_pubkey" > $PUBKEY_FILE
echo ".align 4" >> $PUBKEY_FILE
echo "ed25519_pubkey:" >> $PUBKEY_FILE

cat pub.pem |sed 's/    /\n .byte 0x/g'|\
	sed 's/:/\n .byte 0x/g'|\
	sed -n '/.byte 0x[0-9 a-z]/p' >> $PUBKEY_FILE
cat pub.pem |sed 's/    /0x/g'|\
	sed 's/:/\n0x/g'|\
	sed -n '/0x[0-9 a-z]/p'|\
	sed 's/0x//g'|xargs -I '{}' echo -ne '\x{}' > $PUBKEY_BIN
