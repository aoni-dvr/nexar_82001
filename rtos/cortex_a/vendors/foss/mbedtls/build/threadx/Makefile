# ifneq ($(wildcard $(srctree)/$(src)/../../src),)
# obj-y += ../../
# endif

ifeq ($(CONFIG_BUILD_FOSS_MBEDTLS),y)

# For debug
#KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
#KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))

KBUILD_CFLAGS := $(subst -Werror ,,$(KBUILD_CFLAGS))

obj-y += libmbedtls.a

SRCDIR	:= ../../src
MBEDTLSDIR  := mbedtls-2.16.4

#include $(src)/$(MBEDTLSDIR)/library/Makefile

OBJS_CRYPTO=	aes.o		aesni.o		\
		arc4.o		\
		aria.o		asn1parse.o	asn1write.o	\
		base64.o	bignum.o	blowfish.o	\
		camellia.o	ccm.o		chacha20.o	\
		chachapoly.o	cipher.o	cipher_wrap.o	\
		cmac.o		ctr_drbg.o	des.o		\
		dhm.o		ecdh.o		ecdsa.o		\
		ecjpake.o	ecp.o				\
		ecp_curves.o	entropy.o	entropy_poll.o	\
		error.o		gcm.o		havege.o	\
		hkdf.o						\
		hmac_drbg.o	md.o		md2.o		\
		md4.o		md5.o		md_wrap.o	\
		memory_buffer_alloc.o		nist_kw.o	\
		oid.o		padlock.o	pem.o		\
		pk.o		pk_wrap.o	pkcs12.o	\
		pkcs5.o		pkparse.o	pkwrite.o	\
		platform.o	platform_util.o	poly1305.o	\
		ripemd160.o	rsa_internal.o	rsa.o  		\
		sha1.o		sha256.o	sha512.o	\
		threading.o	timing.o	version.o	\
		version_features.o		xtea.o

OBJS_X509=	certs.o		pkcs11.o	x509.o		\
		x509_create.o	x509_crl.o	x509_crt.o	\
		x509_csr.o	x509write_crt.o	x509write_csr.o

OBJS_TLS=	debug.o		net_sockets.o		\
		ssl_cache.o	ssl_ciphersuites.o	\
		ssl_cli.o	ssl_cookie.o		\
		ssl_srv.o	ssl_ticket.o		\
		ssl_tls.o


libmbedtls-objs += $(patsubst %.o,$(SRCDIR)/$(MBEDTLSDIR)/library/%.o,$(OBJS_CRYPTO))
libmbedtls-objs += $(patsubst %.o,$(SRCDIR)/$(MBEDTLSDIR)/library/%.o,$(OBJS_X509))
libmbedtls-objs += $(patsubst %.o,$(SRCDIR)/$(MBEDTLSDIR)/library/%.o,$(OBJS_TLS))

# Porting to Ambarella platform.
libmbedtls-objs += $(SRCDIR)/ambarella/sys_arch.o

# Test Command
libmbedtls-objs += $(SRCDIR)/mbedtls_test.o

ccflags-y := \
	-I$(srctree)/vendors/foss/mbedtls/src/ambarella/inc \
	-I$(srctree)/vendors/foss/mbedtls/src/$(MBEDTLSDIR)/include \
	-I$(srctree)/vendors/foss/mbedtls/src/$(MBEDTLSDIR)/include/mbedtls \
	-I$(srctree)/soc/io/src/common/inc \
	-I$(srctree)/vendors/ambarella/inc \
	-I$(srctree)/vendors/ambarella/inc/io \
	-I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
	-I${srctree}/svc/ \
	-I$(srctree)/svc/comsvc/print \
	-I$(srctree)/vendors \
	-I$(srctree)

ccflags-y += -DMBEDTLS_USER_CONFIG_FILE='<config.h>'
ccflags-y += -D_FILE_OFFSET_BITS=64

#ifndef CONFIG_CC_USESTD
ccflags-y += -I$(srctree)/vendors/foss/mbedtls/src/ambarella/inc/std
#endif

#ccflags-y += -DMBEDTLS_DEBUG

install-files := libmbedtls.a

endif # $(CONFIG_BUILD_FOSS_MBEDTLS)

