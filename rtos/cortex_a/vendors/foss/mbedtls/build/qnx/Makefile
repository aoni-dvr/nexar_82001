#### QNX

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.

include $(objtree)/include/config/auto.conf

MBEDTLS_ROOT = $(srctree)/vendors/foss/mbedtls
MBEDTLS_DIR  := mbedtls-2.16.4

MBEDTLS_SRC         = $(MBEDTLS_ROOT)/src/
MBEDTLS_AMBA_SRC    = $(MBEDTLS_ROOT)/src/ambarella/
MBEDTLS_LIB_SRC     = $(MBEDTLS_ROOT)/src/$(MBEDTLS_DIR)/library/


EXTRA_SRCVPATH += $(MBEDTLS_SRC)
EXTRA_SRCVPATH += $(MBEDTLS_AMBA_SRC)
EXTRA_SRCVPATH += $(MBEDTLS_LIB_SRC)

SRCS_TEST= mbedtls_test.c
SRCS_AMBA= sys_arch.c

SRCS_CRYPTO=	aes.c		aesni.c		\
		arc4.c		\
		aria.c		asn1parse.c	asn1write.c	\
		base64.c	bignum.c	blowfish.c	\
		camellia.c	ccm.c		chacha20.c	\
		chachapoly.c	cipher.c	cipher_wrap.c	\
		cmac.c		ctr_drbg.c	des.c		\
		dhm.c		ecdh.c		ecdsa.c		\
		ecjpake.c	ecp.c				\
		ecp_curves.c	entropy.c	entropy_poll.c	\
		error.c		gcm.c		havege.c	\
		hkdf.c						\
		hmac_drbg.c	md.c		md2.c		\
		md4.c		md5.c		md_wrap.c	\
		memory_buffer_alloc.c		nist_kw.c	\
		oid.c		padlock.c	pem.c		\
		pk.c		pk_wrap.c	pkcs12.c	\
		pkcs5.c		pkparse.c	pkwrite.c	\
		platform.c	platform_util.c	poly1305.c	\
		ripemd160.c	rsa_internal.c	rsa.c  		\
		sha1.c		sha256.c	sha512.c	\
		threading.c	timing.c	version.c	\
		version_features.c		xtea.c

SRCS_X509=	certs.c		pkcs11.c	x509.c		\
		x509_create.c	x509_crl.c	x509_crt.c	\
		x509_csr.c	x509write_crt.c	x509write_csr.c

SRCS_TLS=	debug.c		net_sockets.c		\
		ssl_cache.c	ssl_ciphersuites.c	\
		ssl_cli.c	ssl_cookie.c		\
		ssl_srv.c	ssl_ticket.c		\
		ssl_tls.c

# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
SRCS =

SRCS += $(SRCS_TEST)
SRCS += $(SRCS_AMBA)
SRCS += $(SRCS_CRYPTO)
SRCS += $(SRCS_X509)
SRCS += $(SRCS_TLS)

EXTRA_INCVPATH += $(MBEDTLS_ROOT)/src/ambarella/inc
EXTRA_INCVPATH += $(MBEDTLS_ROOT)/src/$(MBEDTLS_DIR)/include
EXTRA_INCVPATH += $(MBEDTLS_ROOT)/src/$(MBEDTLS_DIR)/include/mbedtls
EXTRA_INCVPATH += $(srctree)/soc/io/src/common/inc

EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/std
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/svc/
EXTRA_INCVPATH += $(srctree)/vendors
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print
EXTRA_INCVPATH += $(srctree)

CCFLAGS += -DMBEDTLS_USER_CONFIG_FILE='<config.h>'
CCFLAGS += -D_FILE_OFFSET_BITS=64

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
