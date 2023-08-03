/*
 * ambarella/hsmapi.c
 *
 * Ambarella HSM API
 *
 * Copyright (C) 2021, Ambarella Inc.
 */

#include <linux/kernel.h>
#include <linux/module.h>

#include <hsm/api/domboot.h>
#include <hsm/api/debug.h>
#include <hsm/api/information.h>
#include <hsm/api/storage.h>
#include <hsm/api/subdomain.h>
#include <hsm/api/pkgman.h>
#include <hsm/api/ephemeral.h>
#include <hsm/api/crypto.h>
#include <hsm/api/tod.h>
#include <hsm/api/protected.h>
#include <hsm/api/insecure.h>

#include "armhsm.h"

#if defined(ARMHSM_ATF_SHMEM)
#include  <linux/arm-smccc.h>
#define SVC_SCM_FN(s, f)    ((((s) & 0xff) << 8) | ((f) & 0xff))
#define AMBA_SIP_SMC_SVC_HSM    0x1
#define AMBA_SIP_SMC_FN_INIT_HSM_MSG_QUEUE    0x1
#endif

extern int ambarella_scm_hsm_init_queue(void *arg);
int AmbaHSM_Init_HSM_MsgQueue(struct hsm_dom_msgq *arg)
{
#if defined(ARMHSM_ATF_SHMEM)
	return ambarella_scm_hsm_init_queue(arg);
#else
	return armhsm_call(HSM_DOM_MSGQ_ID, arg);
#endif
}
EXPORT_SYMBOL(AmbaHSM_Init_HSM_MsgQueue);

int AmbaHSM_Private_Allocate(struct hsm_prv_alloc *arg)
{
	return armhsm_call(HSM_PRV_ALLOC_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_Private_Allocate);

int AmbaHSM_Domain_Allocate(struct hsm_dom_alloc *arg)
{
	return armhsm_call(HSM_DOM_ALLOC_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_Domain_Allocate);

int AmbaHSM_Load_HSM_Firmware(struct hsm_load_hsm_firmware *arg)
{
	return armhsm_call(HSM_LOAD_HSM_FIRMWARE_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_Load_HSM_Firmware);

int AmbaHSM_Boot_Domain(struct hsm_boot_dom *arg)
{
	return armhsm_call(HSM_BOOT_DOM_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_Boot_Domain);

int AmbaHSM_Boot_DecryptAuth(struct hsm_boot_decryptauth *arg)
{
	return armhsm_call(HSM_BOOT_DECRYPTAUTH_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_Boot_DecryptAuth);

int AmbaHSM_SetDebug(struct hsm_debug *arg)
{
	return armhsm_call(HSM_DEBUG_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_SetDebug);

int AmbaHSM_GetHSMInfo(struct hsm_info *arg)
{
	return armhsm_call(HSM_INFO_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_GetHSMInfo);

int AmbaHSM_GetValidBootSlots(struct hsm_valid_boot_slots *arg)
{
	return armhsm_call(HSM_VALID_BOOT_SLOTS_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_GetValidBootSlots);

int AmbaHSM_SetBootDevices(struct hsm_set_bootdevs *arg)
{
	return armhsm_call(HSM_SET_BOOTDEVS_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_SetBootDevices);

int AmbaHSM_GetFreeStorage(struct hsm_free_storage *arg)
{
	return armhsm_call(HSM_FREE_STORAGE_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_GetFreeStorage);

int AmbaHSM_GetOTPInfo(struct hsm_otp_info *arg)
{
	return armhsm_call(HSM_OTP_INFO_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_GetOTPInfo);

int AmbaHSM_CreateBootKeyObject(struct hsm_create_bootkey_obj *arg)
{
	return armhsm_call(HSM_CREATE_BOOTKEY_OBJ_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_CreateBootKeyObject);

int AmbaHSM_CreateKeyObject(struct hsm_create_key_obj *arg)
{
	return armhsm_call(HSM_CREATE_KEY_OBJ_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_CreateKeyObject);

int AmbaHSM_CopyKeyObject(struct hsm_copy_key_obj *arg)
{
	return armhsm_call(HSM_COPY_KEY_OBJ_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_CopyKeyObject);

int AmbaHSM_CopyBootKeyObject(struct hsm_copy_bootkey_obj *arg)
{
	return armhsm_call(HSM_COPY_BOOTKEY_OBJ_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_CopyBootKeyObject);

int AmbaHSM_GetSubdomains(struct hsm_get_subdomains *arg)
{
	return armhsm_call(HSM_GET_SUBDOMAINS_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_GetSubdomains);

int AmbaHSM_NewSubdomain(struct hsm_new_subdomain *arg)
{
	return armhsm_call(HSM_NEW_SUBDOMAIN_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_NewSubdomain);

int AmbaHSM_RemoveSubdomain(struct hsm_remove_subdomain *arg)
{
	return armhsm_call(HSM_REMOVE_SUBDOMAIN_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_RemoveSubdomain);

int AmbaHSM_GetSubdomainObjects(struct hsm_get_subdomain_objects *arg)
{
	return armhsm_call(HSM_GET_SUBDOMAIN_OBJECTS_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_GetSubdomainObjects);

int AmbaHSM_LookupObject(struct hsm_lookup_object *arg)
{
	return armhsm_call(HSM_LOOKUP_OBJECT_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_LookupObject);

int AmbaHSM_NewAESKey(struct hsm_new_aes_key *arg)
{
	return armhsm_call(HSM_NEW_AES_KEY_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_NewAESKey);

int AmbaHSM_LoadAESKey(struct hsm_load_aes_key *arg)
{
	return armhsm_call(HSM_LOAD_AES_KEY_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_LoadAESKey);

int AmbaHSM_LoadDataObject(struct hsm_load_data_object *arg)
{
	return armhsm_call(HSM_LOAD_DATA_OBJECT_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_LoadDataObject);

int AmbaHSM_NewSubdomainPKCertificate(struct hsm_new_subdomain_pkcert *arg)
{
	return armhsm_call(HSM_NEW_SUBDOMAIN_PKCERT_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_NewSubdomainPKCertificate);

int AmbaHSM_LoadSubdomainPKCertificate(struct hsm_load_subdomain_pkcert *arg)
{
	return armhsm_call(HSM_LOAD_SUBDOMAIN_PKCERT_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_LoadSubdomainPKCertificate);

int AmbaHSM_UpdateSubdomainPKCertificate(struct hsm_update_subdomain_pkcert *arg)
{
	return armhsm_call(HSM_UPDATE_SUBDOMAIN_PKCERT_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_UpdateSubdomainPKCertificate);

int AmbaHSM_InvalidateSubdomainObject(struct hsm_inv_subdomain_object *arg)
{
	return armhsm_call(HSM_INV_SUBDOMAIN_OBJECT_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_InvalidateSubdomainObject);

int AmbaHSM_CreateManifest(struct hsm_create_manifest *arg)
{
	return armhsm_call(HSM_CREATE_MANIFEST_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_CreateManifest);

int AmbaHSM_CreateOTPManifest(struct hsm_create_otp_manifest *arg)
{
	return armhsm_call(HSM_CREATE_OTP_MANIFEST_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_CreateOTPManifest);

int AmbaHSM_CreateDomainManifest(struct hsm_create_domain_manifest *arg)
{
	return armhsm_call(HSM_CREATE_DOMAIN_MANIFEST_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_CreateDomainManifest);

int AmbaHSM_LoadManifest(struct hsm_load_manifest *arg)
{
	return armhsm_call(HSM_LOAD_MANIFEST_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_LoadManifest);

int AmbaHSM_CreatePackage(struct hsm_create_package *arg)
{
	return armhsm_call(HSM_CREATE_PACKAGE_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_CreatePackage);

int AmbaHSM_DecryptPackage(struct hsm_decrypt_package *arg)
{
	return armhsm_call(HSM_DECRYPT_PACKAGE_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_DecryptPackage);

int AmbaHSM_CreateEphemeralPKpair(struct hsm_create_eph_pkpair *arg)
{
	return armhsm_call(HSM_CREATE_EPH_PKPAIR_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_CreateEphemeralPKpair);

int AmbaHSM_LoadTZEphemeralKey(struct hsm_load_tz_eph_key *arg)
{
	return armhsm_call(HSM_LOAD_TZ_EPH_KEY_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_LoadTZEphemeralKey);

int AmbaHSM_CMHProcess(struct hsm_cmh_process *arg)
{
	return armhsm_call(HSM_CMH_PROCESS_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_CMHProcess);

int AmbaHSM_EphemeralECEIS_KEM(struct hsm_eph_eceis_kem *arg)
{
	return armhsm_call(HSM_EPH_ECEIS_KEM_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_EphemeralECEIS_KEM);

int AmbaHSM_CertificateECEIS_KEM(struct hsm_cert_eceis_kem *arg)
{
	return armhsm_call(HSM_CERT_ECEIS_KEM_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_CertificateECEIS_KEM);

int AmbaHSM_EphemeralDHKeyExchange(struct hsm_eph_dh_keyex *arg)
{
	return armhsm_call(HSM_EPH_DH_KEYEX_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_EphemeralDHKeyExchange);

int AmbaHSM_CertificateDHKeyExchange(struct hsm_cert_dh_keyex *arg)
{
	return armhsm_call(HSM_CERT_DH_KEYEX_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_CertificateDHKeyExchange);

int AmbaHSM_EphemeralSign(struct hsm_eph_sign *arg)
{
	return armhsm_call(HSM_EPH_SIGN_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_EphemeralSign);

int AmbaHSM_CertificateSign(struct hsm_cert_sign *arg)
{
	return armhsm_call(HSM_CERT_SIGN_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_CertificateSign);

int AmbaHSM_SignatureVerify(struct hsm_sig_verify *arg)
{
	return armhsm_call(HSM_SIG_VERIFY_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_SignatureVerify);

int AmbaHSM_RNGStreamAllocate(struct hsm_rng_stream_allocate *arg)
{
	return armhsm_call(HSM_RNG_STREAM_ALLOCATE_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_RNGStreamAllocate);

int AmbaHSM_InitializeRNG(struct hsm_initialize_rng *arg)
{
	return armhsm_call(HSM_INITIALIZE_RNG_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_InitializeRNG);

int AmbaHSM_RNGGenerate(struct hsm_rng_generate *arg)
{
	return armhsm_call(HSM_RNG_GENERATE_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_RNGGenerate);

int AmbaHSM_RefreshRNG(struct hsm_refresh_rng *arg)
{
	return armhsm_call(HSM_REFRESH_RNG_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_RefreshRNG);

int AmbaHSM_RelativeAdjust(struct hsm_reladj *arg)
{
	return armhsm_call(HSM_RELADJ_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_RelativeAdjust);

int AmbaHSM_AbsoluteAdjust(struct hsm_absadj *arg)
{
	return armhsm_call(HSM_ABSADJ_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_AbsoluteAdjust);

int AmbaHSM_AbsoluteAdjustRollback(struct hsm_absadj_rollback *arg)
{
	return armhsm_call(HSM_ABSADJ_ROLLBACK_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_AbsoluteAdjustRollback);

int AmbaHSM_ProtectedAddManifest(struct hsm_protected_add_manifest *arg)
{
	return armhsm_call(HSM_PROTECTED_ADD_MANIFEST_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_ProtectedAddManifest);

int AmbaHSM_ProtectedAddFlexidag(struct hsm_protected_add_flexidag *arg)
{
	return armhsm_call(HSM_PROTECTED_ADD_FLEXIDAG_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_ProtectedAddFlexidag);

int AmbaHSM_InsecureKPGen(struct hsm_insecure_kpgen *arg)
{
	return armhsm_call(HSM_INSECURE_KPGEN_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_InsecureKPGen);

int AmbaHSM_InsecureSign(struct hsm_insecure_sign *arg)
{
	return armhsm_call(HSM_INSECURE_SIGN_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_InsecureSign);

int AmbaHSM_InsecureVerify(struct hsm_insecure_verify *arg)
{
	return armhsm_call(HSM_INSECURE_VERIFY_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_InsecureVerify);

int AmbaHSM_InsecureKeyEx(struct hsm_insecure_keyex *arg)
{
	return armhsm_call(HSM_INSECURE_KEYEX_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_InsecureKeyEx);

int AmbaHSM_InsecureCipher(struct hsm_insecure_cipher *arg)
{
	return armhsm_call(HSM_INSECURE_CIPHER_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_InsecureCipher);

int AmbaHSM_PkcExec(struct hsm_pkc_exec *arg)
{
	return armhsm_call(HSM_PKC_EXEC_ID, arg);
}
EXPORT_SYMBOL(AmbaHSM_PkcExec);
