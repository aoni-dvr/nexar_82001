#include ".svc_autogen"

#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaDSP_Capability.h"
#include "AmbaUtility.h"
#include "AmbaSD.h"
#include "SvcPref.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "AmbaKAL.h"
#include "AmbaDMA_Def.h"
#include "SvcTask.h"
#include "AmbaVfs.h"
#include "SvcStgMonitor.h"
#include "SvcUserPrefCustom.h"
#include "SvcPrefCustom.h"

static SVC_USER_PREF_CUSTOM_s *pSvcUserPrefCustom;
static UINT8           InitDone = 0U;

UINT32 SvcUserPrefCustom_Init(void)
{
    ULONG BufAddr = 0U;
    UINT32 BufSize = 0U;

    SvcPrefCustom_GetPrefBuf(&BufAddr, &BufSize);
    AmbaMisra_TypeCast(&pSvcUserPrefCustom, &BufAddr);
    if (SVC_OK != SvcPrefCustom_Load(BufAddr, BufSize)) {
        SvcLog_NG("USRPREF", "SvcPrefCustom_Load failed!!", 0U, 0U);
        AmbaWrap_memset(pSvcUserPrefCustom, 0, sizeof(SVC_USER_PREF_CUSTOM_s));
		return SVC_NG;
    }
    if (pSvcUserPrefCustom->MagicCode != SVC_USER_PREF_CUSTOM_MAGIC_CODE) {
        AmbaWrap_memset(pSvcUserPrefCustom, 0, sizeof(SVC_USER_PREF_CUSTOM_s));
    }
	InitDone = 1U;

	return SVC_OK;
}

UINT32 SvcUserPrefCustom_Get(SVC_USER_PREF_CUSTOM_s** ppSvcUserPrefCustom)
{
    UINT32 Rval = SVC_OK;

    if (InitDone == 1U) {
        *ppSvcUserPrefCustom = pSvcUserPrefCustom;
    } else {
        //SvcLog_NG("USRPREF", "Need to do SvcUserPrefCustom_Init first!!", 0U, 0U);
        *ppSvcUserPrefCustom = NULL;
        Rval = SVC_NG;
    }
    return Rval;
}

void SvcUserPrefCustom_Save(void)
{
    UINT32          PrefBufSize;
    ULONG           PrefBufAddr;
    SVC_USER_PREF_CUSTOM_s *pSvcUserPrefCustom;

    if (SVC_OK != SvcUserPrefCustom_Get(&pSvcUserPrefCustom)) {
        SvcLog_NG("USRPREF", "SvcUserPref_GetCustomer() failed!!", 0U, 0U);
    } else {
        pSvcUserPrefCustom->MagicCode = SVC_USER_PREF_CUSTOM_MAGIC_CODE;
        SvcPrefCustom_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
        if (SVC_OK != SvcPrefCustom_Save(PrefBufAddr, PrefBufSize)) {
            SvcLog_NG("USRPREF", "SvcPref_Save_Customer failed!!", 0U, 0U);
        }
    }
}

