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
#include "Amba_AsilAwbCtrl.h"

extern UINT32 AmbaImgProc_AWBGetPreWBGain(UINT32 ViewID, UINT32 ExpNo, UINT32 Mode, AMBA_IK_WB_GAIN_s *pAWBGain);
extern UINT32 AmbaImgProc_AWBSetASILWBGain(UINT32 ViewID, UINT32 ExpNo, UINT32 Mode, const AMBA_IK_WB_GAIN_s *pAWBGain);
extern UINT32 AmbaImgProc_AWBGetASILWBGain(UINT32 ViewID, UINT32 ExpNo, UINT32 Mode, AMBA_IK_WB_GAIN_s *pAWBGain);


UINT32 AmbaImgProc_ASILAWBInit(UINT32 ViewId)
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

UINT32 AmbaImgProc_ASILAWBControl(UINT32 ViewId)
{
    UINT32 RVal = 0U;
    AMBA_IK_WB_GAIN_s AWBGain,PreAWBGain;
    (void)AmbaImgProc_AWBGetWBGain(ViewId,0,0,&AWBGain);
    (void)AmbaImgProc_AWBGetPreWBGain(ViewId,0,0,&PreAWBGain);
    AmbaPrint_PrintUInt5("#### Amba_ASILAwb_Ctrl Pre : %5d, %5d, %5d", PreAWBGain.GainB, PreAWBGain.GainG, PreAWBGain.GainR, 0U, 0U);
    AmbaPrint_PrintUInt5("#### Amba_ASILAwb_Ctrl Now : %5d, %5d, %5d", AWBGain.GainB, AWBGain.GainG, AWBGain.GainR, 0U, 0U);
    if(RVal==0U){
        /*MISRAC*/
    }
    return RVal;
}
