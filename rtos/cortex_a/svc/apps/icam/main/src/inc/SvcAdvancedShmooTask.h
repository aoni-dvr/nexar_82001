#ifndef SVC_ADV_SHMOO_TASK
#define SVC_ADV_SHMOO_TASK

UINT32 SvcAdvancedShmooTask_Start(void);
void SvcAdvancedShmooTask_StatusCheck(UINT32 StatusID, ULONG ResultAddr, UINT32 *pHaveError);
void SvcCmdAdvShmoo_Install(void);

#endif