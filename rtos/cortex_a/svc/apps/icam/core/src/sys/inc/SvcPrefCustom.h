#ifndef __SVC_PREF_CUSTOM_H__
#define __SVC_PREF_CUSTOM_H__

UINT32 SvcPrefCustom_Save(ULONG PrefBufAddr, UINT32 PrefBufSize);
UINT32 SvcPrefCustom_Load(ULONG PrefBufAddr, UINT32 PrefBufSize);
void SvcPrefCustom_GetPrefBuf(ULONG *pBufAddr, UINT32 *pBufSize);

#endif /* __SVC_PREF_CUSTOM_H__ */

