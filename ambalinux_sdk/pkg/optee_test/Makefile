ifeq ($O,)
out-dir := $(CURDIR)/out
else
include scripts/common.mk
out-dir := $(call strip-trailing-slashes-and-dots,$(O))
ifeq ($(out-dir),)
$(error invalid output directory (O=$(O)))
endif
endif

-include $(TA_DEV_KIT_DIR)/host_include/conf.mk
-include $(OPTEE_CLIENT_EXPORT)/include/optee_client_config.mk

ifneq ($V,1)
	q := @
	echo := @echo
else
	q :=
	echo := @:
endif
# export 'q', used by sub-makefiles.
export q

# If _HOST or _TA specific compilers are not specified, then use CROSS_COMPILE
CROSS_COMPILE_HOST ?= $(CROSS_COMPILE)
CROSS_COMPILE_TA ?= $(CROSS_COMPILE)

.PHONY: all
ifneq ($(wildcard $(TA_DEV_KIT_DIR)/host_include/conf.mk),)
all: xtest ta test_plugin test_data_encryption_aes data_at_rest_test test_program_otp test_secure_storage test_secure_credential_provisioning openssl_sign_engine_test secure_storage_simple_test \
		optee_tls_test data_encryption_aes_v2_test data_encryption_aes_v2_verify
else
all:
	$(q)echo "TA_DEV_KIT_DIR is not correctly defined" && false
endif

.PHONY: xtest
xtest:
	$(q)$(MAKE) -C host/xtest CROSS_COMPILE="$(CROSS_COMPILE_HOST)" \
			     --no-builtin-variables \
			     O=$(out-dir) \
			     $@

.PHONY:test_data_encryption_aes
test_data_encryption_aes:
	$(q)$(MAKE) -C host/test_data_encryption_aes CROSS_COMPILE="$(CROSS_COMPILE_HOST)" \
			     --no-builtin-variables \
			     O=$(out-dir) \
			     $@

.PHONY: data_at_rest_test
data_at_rest_test:
	$(q)$(MAKE) -C host/data_at_rest_test CROSS_COMPILE="$(CROSS_COMPILE_HOST)" \
			     --no-builtin-variables \
			     O=$(out-dir) \
			     $@

.PHONY: test_secure_credential_provisioning
test_secure_credential_provisioning:
	$(q)$(MAKE) -C host/test_secure_credential_provisioning CROSS_COMPILE="$(CROSS_COMPILE_HOST)" \
			     --no-builtin-variables \
			     O=$(out-dir) \
			     $@

.PHONY: test_secure_storage
test_secure_storage:
	$(q)$(MAKE) -C host/test_secure_storage CROSS_COMPILE="$(CROSS_COMPILE_HOST)" \
			     --no-builtin-variables \
			     O=$(out-dir) \
			     $@

.PHONY: test_program_otp
test_program_otp:
	$(q)$(MAKE) -C host/test_program_otp CROSS_COMPILE="$(CROSS_COMPILE_HOST)" \
			     --no-builtin-variables \
			     O=$(out-dir) \
			     $@

.PHONY:openssl_sign_engine_test
openssl_sign_engine_test:
	$(q)$(MAKE) -C host/openssl_sign_engine CROSS_COMPILE="$(CROSS_COMPILE_HOST)" \
			     --no-builtin-variables \
			     O=$(out-dir) \
			     $@

.PHONY:secure_storage_simple_test
secure_storage_simple_test:
	$(q)$(MAKE) -C host/secure_storage_simple_test CROSS_COMPILE="$(CROSS_COMPILE_HOST)" \
			     --no-builtin-variables \
			     O=$(out-dir) \
			     $@

.PHONY:optee_tls_test
optee_tls_test:
	$(q)$(MAKE) -C host/optee_tls_test CROSS_COMPILE="$(CROSS_COMPILE_HOST)" \
			     --no-builtin-variables \
			     O=$(out-dir) \
			     $@

.PHONY:data_encryption_aes_v2_test
data_encryption_aes_v2_test:
	$(q)$(MAKE) -C host/data_encryption_aes_v2 CROSS_COMPILE="$(CROSS_COMPILE_HOST)" \
			     --no-builtin-variables \
			     O=$(out-dir) \
			     $@

.PHONY:data_encryption_aes_v2_verify
data_encryption_aes_v2_verify:
	$(q)$(MAKE) -C host/data_encryption_aes_v2_verify CROSS_COMPILE="$(CROSS_COMPILE_HOST)" \
			     --no-builtin-variables \
			     O=$(out-dir) \
			     $@

.PHONY: ta
ta:
	$(q)$(MAKE) -C ta CROSS_COMPILE="$(CROSS_COMPILE_TA)" \
			  O=$(out-dir) \
			  $@

.PHONY: test_plugin
test_plugin:
	$(q)$(MAKE) -C host/supp_plugin CROSS_COMPILE="$(CROSS_COMPILE_HOST)" \
			     O=$(out-dir)

.PHONY: clean
ifneq ($(wildcard $(TA_DEV_KIT_DIR)/host_include/conf.mk),)
clean:
	$(q)$(MAKE) -C host/xtest O=$(out-dir) $@
	$(q)$(MAKE) -C host/test_data_encryption_aes O=$(out-dir) $@
	$(q)$(MAKE) -C host/data_at_rest_test O=$(out-dir) $@
	$(q)$(MAKE) -C host/test_secure_credential_provisioning O=$(out-dir) $@
	$(q)$(MAKE) -C host/test_program_otp O=$(out-dir) $@
	$(q)$(MAKE) -C host/test_secure_storage O=$(out-dir) $@
	$(q)$(MAKE) -C host/openssl_sign_engine O=$(out-dir) $@
	$(q)$(MAKE) -C host/secure_storage_simple_test O=$(out-dir) $@
	$(q)$(MAKE) -C host/optee_tls_test O=$(out-dir) $@
	$(q)$(MAKE) -C host/data_encryption_aes_v2 O=$(out-dir) $@
	$(q)$(MAKE) -C host/data_encryption_aes_v2_verify O=$(out-dir) $@
	$(q)$(MAKE) -C ta O=$(out-dir) $@
	$(q)$(MAKE) -C host/supp_plugin O=$(out-dir) $@
else
clean:
	$(q)echo "TA_DEV_KIT_DIR is not correctly defined"
	$(q)echo "You can remove manually $(out-dir)"
endif

.PHONY: checkpatch checkpatch-staging checkpatch-working
checkpatch: checkpatch-staging checkpatch-working

checkpatch-working:
	@./scripts/checkpatch.sh

checkpatch-staging:
	@./scripts/checkpatch.sh --cached

install: all
	$(q)mkdir -p ${DESTDIR}/lib/optee_armtz
	$(q)mkdir -p ${DESTDIR}/bin
	$(echo) '  INSTALL ${DESTDIR}/lib/optee_armtz'
	$(q)find $(out-dir)/ta -name \*.ta -exec cp -a {} ${DESTDIR}/lib/optee_armtz \;
	$(echo) '  INSTALL ${DESTDIR}/bin'
	$(q)cp -a $(out-dir)/xtest/xtest ${DESTDIR}/bin
	##$(echo) '  INSTALL ${DESTDIR}/$(CFG_TEE_PLUGIN_LOAD_PATH)'
	##$(q)mkdir -p ${DESTDIR}/$(CFG_TEE_PLUGIN_LOAD_PATH)
	##$(q)cp $(out-dir)/supp_plugin/*.plugin ${DESTDIR}/$(CFG_TEE_PLUGIN_LOAD_PATH)
	$(q)cp -a $(out-dir)/test_data_encryption_aes/test_data_encryption_aes ${DESTDIR}/bin
	$(q)cp -a $(out-dir)/data_at_rest_test/data_at_rest_test ${DESTDIR}/bin
	$(q)cp -a $(out-dir)/test_program_otp/test_program_otp ${DESTDIR}/bin
	$(q)cp -a $(out-dir)/test_secure_storage/test_secure_storage ${DESTDIR}/bin
	$(q)cp -a $(out-dir)/test_secure_credential_provisioning/test_secure_credential_provisioning ${DESTDIR}/bin
	$(q)cp -a $(out-dir)/openssl_sign_engine_test/openssl_sign_engine_test ${DESTDIR}/bin
	$(q)cp -a $(out-dir)/secure_storage_simple_test/secure_storage_simple_test ${DESTDIR}/bin
	##$(q)cp -a $(out-dir)/optee_tls_test/optee_tls_test ${DESTDIR}/bin
	$(q)cp -a $(out-dir)/data_encryption_aes_v2/data_encryption_aes_v2_test ${DESTDIR}/bin
	$(q)cp -a $(out-dir)/data_encryption_aes_v2_verify/data_encryption_aes_v2_verify ${DESTDIR}/bin

.PHONY: cscope
cscope:
	$(echo) '  CSCOPE  .'
	${q}rm -f cscope.*
	${q}find $(PWD) -name "*.[ch]" -o -name "*.cpp" | grep -v /package/ > cscope.files
	${q}cscope -b -q -k
