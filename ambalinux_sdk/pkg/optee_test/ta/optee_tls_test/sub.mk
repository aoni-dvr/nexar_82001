global-incdirs-y += include

srcs-$(CFG_TA_MBEDTLS) += optee_tls_test_ta_entry.c
srcs-$(CFG_TA_MBEDTLS) += optee_tls_test_ta_handle.c
srcs-y += optee_tls_test_ta_socket_handle.c

libnames += mbedtls
libdirs += $(ta-dev-kit-dir$(sm))/lib
libdeps += $(ta-dev-kit-dir$(sm))/lib/libmbedtls.a