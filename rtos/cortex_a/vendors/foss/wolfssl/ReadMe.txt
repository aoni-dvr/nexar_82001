Note
=====

1. wolfSSL license issue (TODO)
2. wolfSSL need malloc/free (stdlib.h), string.h, stdio.h
   Below example we build wolfSSL with few libc.a APIs,
   so need C run-time libraries and syscall APIs,
   i.e. sample codes like vendors/foss/stdsupp/, vendors/foss/syscall/
   And need to use original libc.a from toolchain.


Parepare
=========

1. Download wolfssl-4.8.1.zip https://www.wolfssl.com/download/
2. cd PathA/rtos/cortex_a/
3. unzip PathB/wolfssl-4.8.1.zip -d vendors/foss/wolfssl/
4. cp vendors/foss/wolfssl/wolfssl-4.8.1/wolfcrypt/benchmark/benchmark.c vendors/foss/wolfssl/Source/
5. cp vendors/foss/wolfssl/wolfssl-4.8.1/IDE/GCC-ARM/Header/user_settings.h vendors/foss/wolfssl/Header/
6. patch -p1 < vendors/foss/wolfssl/patch/wolfssl-4.8.1-amba01.diff
7. (For iout) patch -d svc -p1 < vendors/foss/wolfssl/patch/svc.diff
8. (Option) patch -p1 < vendors/foss/wolfssl/patch/syscall.diff


Build for cv22 iout
====================

1. source build/maintenance/env/env_set.sh cv22
2. cp `arm-none-eabi-gcc -mlittle-endian -marm -mabi=aapcs-linux -mno-thumb-interwork -march=armv8-a+crc -mcpu=cortex-a53 -mfloat-abi=hard -mfpu=crypto-neon-fp-armv8 -print-file-name=libc.a` vendors/ambarella/wrapper/std/bin/32b/libc.a.bin
3. make cv22/cv22_svc_iout_bub_nand_defconfig
4. make CONFIG_CC_USESTD=y vendors/foss/stdsupp/
5. make CONFIG_CC_USESTD=y vendors/foss/syscall/
6. make vendors/foss/wolfssl/
7. make -j



Results
========

a:\> wbenchmark
------------------------------------------------------------------------------
 wolfSSL version 4.8.1
------------------------------------------------------------------------------
wolfCrypt Benchmark (block bytes 1024, min 1.0 sec each)
RNG                 17 MB took 1.000 seconds,   17.041 MB/s
AES-128-CBC-enc    378 MB took 1.000 seconds,  378.345 MB/s
AES-128-CBC-dec    366 MB took 1.000 seconds,  365.601 MB/s
AES-192-CBC-enc    334 MB took 1.000 seconds,  333.521 MB/s
AES-192-CBC-dec    323 MB took 1.000 seconds,  323.145 MB/s
AES-256-CBC-enc    271 MB took 1.000 seconds,  270.947 MB/s
AES-256-CBC-dec    247 MB took 1.000 seconds,  247.363 MB/s
AES-128-GCM-enc     18 MB took 1.000 seconds,   17.944 MB/s
AES-128-GCM-dec     18 MB took 1.000 seconds,   17.749 MB/s
AES-192-GCM-enc     18 MB took 1.001 seconds,   17.658 MB/s
AES-192-GCM-dec     17 MB took 1.001 seconds,   17.463 MB/s
AES-256-GCM-enc     17 MB took 1.000 seconds,   17.432 MB/s
AES-256-GCM-dec     17 MB took 1.001 seconds,   17.219 MB/s
GMAC Small          40 MB took 1.000 seconds,   39.903 MB/s
SHA                  8 MB took 1.001 seconds,    7.561 MB/s
SHA-256            256 MB took 1.000 seconds,  255.762 MB/s
HMAC-SHA             8 MB took 1.002 seconds,    7.505 MB/s
HMAC-SHA256        245 MB took 1.000 seconds,  245.215 MB/s
RSA     2048 public         68 ops took 1.007 sec, avg 14.809 ms, 67.527 ops/sec
RSA     2048 private         2 ops took 1.678 sec, avg 839.000 ms, 1.192 ops/sec
DH      2048 key gen         6 ops took 1.050 sec, avg 175.000 ms, 5.714 ops/sec
DH      2048 agree           4 ops took 1.423 sec, avg 355.750 ms, 2.811 ops/sec
ECC   [      SECP256R1]   256 key gen         7 ops took 1.091 sec, avg 155.857 ms, 6.416 ops/sec
ECDHE [      SECP256R1]   256 agree           8 ops took 1.246 sec, avg 155.750 ms, 6.421 ops/sec
ECDSA [      SECP256R1]   256 sign            8 ops took 1.256 sec, avg 157.000 ms, 6.369 ops/sec
ECDSA [      SECP256R1]   256 verify         10 ops took 1.046 sec, avg 104.600 ms, 9.560 ops/sec
Benchmark complete



