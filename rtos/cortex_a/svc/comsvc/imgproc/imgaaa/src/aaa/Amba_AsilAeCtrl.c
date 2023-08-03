#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"
#ifdef CONFIG_LINUX
#include "AmbaWrap.h"
#endif
#include "AmbaDSP_ImageFilter.h"
#include "AmbaImg_Proc.h"
#include "AmbaImg_AeAwb.h"
#include "AmbaImg_AaaDef.h"
#include "AmbaImg_External_CtrlFunc.h"
#include "Amba_AsilAeCtrl.h"

extern UINT32 AmbaImgProc_AEGetPreExpInfo(UINT32 ViewID, UINT32 ExpNo, UINT32 Mode, AMBA_AE_INFO_s *pAEInfo);
extern UINT32 AmbaImgProc_AEGetASILExpInfo(UINT32 ViewID, UINT32 ExpNo, UINT32 Mode, AMBA_AE_INFO_s *pAEInfo);
extern UINT32 AmbaImgProc_AESetASILExpInfo(UINT32 ViewID, UINT32 ExpNo, UINT32 Mode, const AMBA_AE_INFO_s *pAEInfo);
extern void AmbaImgProc_PrintAeInfo(UINT32 frameNum, AMBA_AE_INFO_s AeInfo);


UINT32 AmbaImgProc_ASILAEInit(UINT32 ViewId)
{
    UINT32 RVal = 0U;
    if(ViewId==0U){
        /*MISRAC*/
    }
    if(RVal==0U){
        /*MISRAC*/
    }
    return RVal;
}

UINT32 AmbaImgProc_ASILAEControl(UINT32 ViewId)
{
    UINT32 RVal = 0U;
    AMBA_AE_INFO_s AeInfo,PreAeInfo;
    (void)AmbaImgProc_AEGetExpInfo(ViewId, 0, 0, &AeInfo);
    (void)AmbaImgProc_AEGetPreExpInfo(ViewId, 0, 0, &PreAeInfo);
    AmbaPrint_PrintUInt5("  ===Amba_ASILAe_Ctrl Pre", 0U, 0U, 0U, 0U, 0U);
    AmbaImgProc_PrintAeInfo(0, PreAeInfo);
    AmbaPrint_PrintUInt5("  ===Amba_ASILAe_Ctrl Now", 0U, 0U, 0U, 0U, 0U);
    AmbaImgProc_PrintAeInfo(0, AeInfo);

    if(RVal==0U){
        /*MISRAC*/
    }
    return RVal;
}
