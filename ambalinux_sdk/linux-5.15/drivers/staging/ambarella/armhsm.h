/*
 * ambarella/armhsm.h
 *
 * Copyright (C) 2021, Ambarella Inc.
 */

#ifndef AMBARELLA_ARMHSM_H
#define AMBARELLA_ARMHSM_H

#include <hsm/api/struct.h>
#include <hsm/api/linux.h>
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
#include <hsm/api/provider.h>

#ifndef ARMHSM_USE_DMA
#define ARMHSM_USE_DMA	1
#endif

#ifndef ARMHSM_ATF_SHMEM
#define ARMHSM_ATF_SHMEM
#endif

#include <linux/semaphore.h>

extern uint32_t hsm_api_hash;

typedef int(*API_FUNC)(void *arg);

struct api_id2f {
	uint8_t  cmd_id;
	API_FUNC api_func;
};

extern const struct api_id2f api_id2f[];

extern unsigned int my_hsm_domain(void);

struct hsmnvm {
	pid_t pid;
	struct semaphore req_sem;
	struct semaphore reply_sem;
	struct hsm_register_nvm_pro nvm_pro;
	struct hsm_wait_n_send_req nvm_req;
};

extern int armhsm_call(uint32_t, void *);
extern int armhsm_comm_instance(void);
extern struct comm_inst_info *armhsm_comm(void);

extern int hsmnvm_init(struct hsmnvm *hsmnvm);
extern int hsmnvm_cleanup(void);
extern int hsmnvm_set_state(unsigned int online);
extern int hsmnvm_get_nvm_info(struct nvmfs_info *);
extern int hsmnvm_read_rpmb(struct nvmfs_read_rpmb *);
extern int hsmnvm_write_rpmb(struct nvmfs_write_rpmb *);
extern int hsmnvm_read_obj(struct nvmfs_read_obj *);
extern int hsmnvm_write_obj(struct nvmfs_write_obj *);

extern int AmbaHSM_Init_HSM_MsgQueue(struct hsm_dom_msgq *);
extern int AmbaHSM_Private_Allocate(struct hsm_prv_alloc *);
extern int AmbaHSM_Domain_Allocate(struct hsm_dom_alloc *);
extern int AmbaHSM_Load_HSM_Firmware(struct hsm_load_hsm_firmware *);
extern int AmbaHSM_Boot_Domain(struct hsm_boot_dom *);
extern int AmbaHSM_Boot_DecryptAuth(struct hsm_boot_decryptauth *);
extern int AmbaHSM_SetDebug(struct hsm_debug *);
extern int AmbaHSM_GetHSMInfo(struct hsm_info *);
extern int AmbaHSM_GetValidBootSlots(struct hsm_valid_boot_slots *);
extern int AmbaHSM_SetBootDevices(struct hsm_set_bootdevs *);
extern int AmbaHSM_GetFreeStorage(struct hsm_free_storage *);
extern int AmbaHSM_GetOTPInfo(struct hsm_otp_info *);
extern int AmbaHSM_CreateBootKeyObject(struct hsm_create_bootkey_obj *);
extern int AmbaHSM_CreateKeyObject(struct hsm_create_key_obj *);
extern int AmbaHSM_CopyKeyObject(struct hsm_copy_key_obj *);
extern int AmbaHSM_CopyBootKeyObject(struct hsm_copy_bootkey_obj *);
extern int AmbaHSM_GetSubdomains(struct hsm_get_subdomains *);
extern int AmbaHSM_NewSubdomain(struct hsm_new_subdomain *);
extern int AmbaHSM_RemoveSubdomain(struct hsm_remove_subdomain *);
extern int AmbaHSM_GetSubdomainObjects(struct hsm_get_subdomain_objects *);
extern int AmbaHSM_LookupObject(struct hsm_lookup_object *);
extern int AmbaHSM_NewAESKey(struct hsm_new_aes_key *);
extern int AmbaHSM_LoadAESKey(struct hsm_load_aes_key *);
extern int AmbaHSM_LoadDataObject(struct hsm_load_data_object *);
extern int AmbaHSM_NewSubdomainPKCertificate(struct hsm_new_subdomain_pkcert *);
extern int AmbaHSM_LoadSubdomainPKCertificate(struct hsm_load_subdomain_pkcert *);
extern int AmbaHSM_UpdateSubdomainPKCertificate(struct hsm_update_subdomain_pkcert *);
extern int AmbaHSM_InvalidateSubdomainObject(struct hsm_inv_subdomain_object *);
extern int AmbaHSM_CreateManifest(struct hsm_create_manifest *);
extern int AmbaHSM_CreateOTPManifest(struct hsm_create_otp_manifest *);
extern int AmbaHSM_CreateDomainManifest(struct hsm_create_domain_manifest *);
extern int AmbaHSM_LoadManifest(struct hsm_load_manifest *);
extern int AmbaHSM_CreatePackage(struct hsm_create_package *);
extern int AmbaHSM_DecryptPackage(struct hsm_decrypt_package *);
extern int AmbaHSM_CreateEphemeralPKpair(struct hsm_create_eph_pkpair *);
extern int AmbaHSM_LoadTZEphemeralKey(struct hsm_load_tz_eph_key *);
extern int AmbaHSM_CMHProcess(struct hsm_cmh_process *);
extern int AmbaHSM_EphemeralECEIS_KEM(struct hsm_eph_eceis_kem *);
extern int AmbaHSM_CertificateECEIS_KEM(struct hsm_cert_eceis_kem *);
extern int AmbaHSM_EphemeralDHKeyExchange(struct hsm_eph_dh_keyex *);
extern int AmbaHSM_CertificateDHKeyExchange(struct hsm_cert_dh_keyex *);
extern int AmbaHSM_EphemeralSign(struct hsm_eph_sign *);
extern int AmbaHSM_CertificateSign(struct hsm_cert_sign *);
extern int AmbaHSM_SignatureVerify(struct hsm_sig_verify *);
extern int AmbaHSM_RNGStreamAllocate(struct hsm_rng_stream_allocate *);
extern int AmbaHSM_InitializeRNG(struct hsm_initialize_rng *);
extern int AmbaHSM_RNGGenerate(struct hsm_rng_generate *);
extern int AmbaHSM_RefreshRNG(struct hsm_refresh_rng *);
extern int AmbaHSM_RelativeAdjust(struct hsm_reladj *);
extern int AmbaHSM_AbsoluteAdjust(struct hsm_absadj *);
extern int AmbaHSM_AbsoluteAdjustRollback(struct hsm_absadj_rollback *);
extern int AmbaHSM_ProtectedAddManifest(struct hsm_protected_add_manifest *);
extern int AmbaHSM_ProtectedAddFlexidag(struct hsm_protected_add_flexidag *);
extern int AmbaHSM_InsecureKPGen(struct hsm_insecure_kpgen *);
extern int AmbaHSM_InsecureSign(struct hsm_insecure_sign *);
extern int AmbaHSM_InsecureVerify(struct hsm_insecure_verify *);
extern int AmbaHSM_InsecureKeyEx(struct hsm_insecure_keyex *);
extern int AmbaHSM_InsecureCipher(struct hsm_insecure_cipher *);
extern int AmbaHSM_PkcExec(struct hsm_pkc_exec *);

#endif
