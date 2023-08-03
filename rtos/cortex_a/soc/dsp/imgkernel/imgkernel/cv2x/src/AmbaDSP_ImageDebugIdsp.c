/*
* Copyright (c) 2020 Ambarella International LP
*
* This file and its contents ("Software") are protected by intellectual
* property rights including, without limitation, U.S. and/or foreign
* copyrights. This Software is also the confidential and proprietary
* information of Ambarella International LP and its licensors. You may not use, reproduce,
* disclose, distribute, modify, or otherwise prepare derivative works of this
* Software or any portion thereof except pursuant to a signed license agreement
* or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
* In the absence of such an agreement, you agree to promptly notify and return
* this Software to Ambarella International LP.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
* MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#include "AmbaTypes.h"
#include "AmbaFS.h"

#include "AmbaPrint.h" //AmbaPrint
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageUtility.h"

#define amba_ik_system_print(...) //AmbaPrint_PrintUInt5  //TBD

#if (defined(__unix__) && !defined(__QNX__) && !defined(CONFIG_LINUX))
#include "stdio.h"
#include "string.h"
#define amba_ik_system_memcpy memcpy   //TBD

#else

#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "RefWrap.h"

#define amba_ik_system_memcpy AmbaWrap_memcpy   //TBD
#define amba_ik_system_print_uint32_5 AmbaPrint_PrintUInt5//(...) 0//AmbaPrint
#define amba_ik_system_print_str_5 AmbaPrint_PrintStr5//(...) 0//AmbaPrint

#define MAX_NUM_IDSP_CONFIG     8
#define CV1_MAX_IDSP_CFG (128*1234)
//uint32  ik_debug_level = 0;
//uint32  ik_warp_debug_level = 0;

/* ============== IdspCfg ============== */
/* debug_cfg_info_t */
typedef struct {
    UINT32 CfgAddr;
    UINT32 CfgSize;
} IDSP_CFG_INFO_s;

/* debug_yuv_info_t */
typedef struct {
    UINT32 LumaAddr;
    UINT32 ChromaAddr;
    UINT16 LumaPitch;
    UINT16 LumaWidth;
    UINT16 LumaHeight;
    UINT16 ChromaPitch;
    UINT16 ChromaWidth;
    UINT16 ChromaHeight;
} IDSP_YUV_INFO_s;

/* debug_buf_info_t */
typedef struct {
    UINT32 Addr;
    UINT16 Pitch;
    UINT16 Width;
    UINT16 Height;
    UINT16 reserved;
} IDSP_BUF_INFO_s;

/* debug_c2y_info_t */
typedef struct {
    UINT32 CapSeqNo;

    IDSP_CFG_INFO_s Sec4Cfg;
    IDSP_CFG_INFO_s Sec2Cfg;

    IDSP_YUV_INFO_s Sec2R2y;
    IDSP_BUF_INFO_s Sec2Me1;
} IDSP_C2Y_INFO_s;

/* debug_me1_smooth_info_t */
typedef struct {
    UINT32 CapSeqNo;

    IDSP_CFG_INFO_s Sec2Cfg;
    IDSP_CFG_INFO_s Sec6Cfg;
    IDSP_CFG_INFO_s Sec5Cfg;

    IDSP_BUF_INFO_s vWarpMe1;
    IDSP_BUF_INFO_s Me1Dn3;
} IDSP_ME1_SMOOTH_INFO_s;

/* debug_md_info_t */
typedef struct {
    UINT32 CapSeqNo;

    IDSP_CFG_INFO_s Sec2Cfg[3U]; // 0: mda, 1: mdb, 2: mdc
    IDSP_CFG_INFO_s Sec6Cfg;
    IDSP_CFG_INFO_s Sec5Cfg;
    IDSP_CFG_INFO_s MctfCfg;

    IDSP_BUF_INFO_s MdMbB;
    IDSP_BUF_INFO_s MdMbC;
    IDSP_BUF_INFO_s MoAsf;
} IDSP_MD_INFO_s;

typedef struct {
    UINT32 CapSeqNo;

    IDSP_CFG_INFO_s MctfCfg;
    IDSP_CFG_INFO_s MctsCfg;

    IDSP_BUF_INFO_s Sec3Me1;
    IDSP_YUV_INFO_s Mctf;
    IDSP_YUV_INFO_s Mcts;
    IDSP_BUF_INFO_s Mctf2bit;
} IDSP_VWARP_MCTF_INFO_s;

#define IDSP_MAX_AAA_SLICE_NUM  (16U)
typedef struct {
    UINT32 CapSeqNo;
    UINT16 SliceX;
    UINT16 SliceY;
    UINT32 Addr[IDSP_MAX_AAA_SLICE_NUM];
} IDSP_AAA_STAT_s;

#define IDSP_MAX_AAA_EXP_NUM  (3U)
typedef struct {
    UINT32 ChannelId;
    IDSP_C2Y_INFO_s C2Y;
    IDSP_VWARP_MCTF_INFO_s vWarpMctf;
    IDSP_ME1_SMOOTH_INFO_s Me1Smooth;
    IDSP_MD_INFO_s Md;
    IDSP_AAA_STAT_s CfaAAA;
    IDSP_AAA_STAT_s PgAAA;
    IDSP_AAA_STAT_s HistAAA[IDSP_MAX_AAA_EXP_NUM];
} IDSP_INFO_s;
/* ===================================== */

static UINT32 IK_DumpCropedBuffer(UINT8 *pDumpAddress,char *FileName, UINT32  pitch, UINT32  width, UINT32  height, UINT8 *pWorkingBuf)
{
    UINT32  Rval = IK_OK;
    UINT32  i;
    AMBA_FS_FILE *Fid;
    char *Fmode = "w";
    UINT32  out_width;
    UINT32 NumSuccess;

    if((pDumpAddress == (UINT8 *)0x0 )||(pDumpAddress == (UINT8 *)0xFFFFFFFF)) {
        amba_ik_system_print("[IK] The File %s dump from %p fail, please check the flow or DSP log", FileName, pDumpAddress);
        return IK_ERR_0000; // TBD
    }

    out_width = width;

    for(i =0; i < height; i++) {
        amba_ik_system_memcpy((void *)(pWorkingBuf + (i*out_width)), (void *) (pDumpAddress + (i*pitch)), sizeof(UINT8)*out_width);
    }
    Rval = AmbaFS_FileOpen(FileName, Fmode, &Fid);
    if (Fid == NULL ) {
        amba_ik_system_print("[IK] File open fail. Skip dumping debug data %s", FileName);
        return IK_ERR_0000; // TBD
    }
    Rval = AmbaFS_FileWrite((void *) (pWorkingBuf), sizeof(UINT8), out_width*height, Fid, &NumSuccess);

    if (NumSuccess != (out_width*height) ) {
        amba_ik_system_print("[IK] File write fail. The content in file:%s is not reliable.", FileName);
        return IK_ERR_0000;  // TBD
    }

    Rval = AmbaFS_FileClose(Fid);

    return Rval;

}

UINT32 mctf_cfg_addr = 0;
UINT32 mcts_cfg_addr = 0;

UINT32 IK_DebugVideoDump(const AMBA_IK_MODE_CFG_s *pMode, char *dump_folder, INT32 dump_level, UINT8 *p_buffer)
{
    extern UINT32 AmbaDSP_LiveviewGetIDspCfg(UINT16 ViewZoneId, UINT32 *CfgAddr);//__attribute__((weak));
    UINT32  Rval = IK_OK;
    IDSP_INFO_s IdspInfo;
    UINT8 ViewZoneID = pMode->ContextId;
    UINT32 pCfgAddr = 0;
    char *Fmode = "w";
    char FileName[128];
    //char *dump_folder = "c:\\dump";
    AMBA_FS_FILE *Fid;
    UINT8 *pDumpAddress = 0;
    INT32 DumpSize = 0;
    AMBA_IK_ABILITY_s Ability;
    AmbaIK_GetContextAbility(pMode, &Ability);

    amba_ik_system_print("[IK] start video dump...id %d level %d \n", ViewZoneID, dump_level);

    if (AmbaDSP_LiveviewGetIDspCfg(ViewZoneID, &pCfgAddr) != 0) {
        amba_ik_system_print("[IK] VPROC_GET_EXT_DATA_INFO error\n");
        return -1;
    } else {
        amba_ik_system_memcpy(&IdspInfo, (void*)pCfgAddr, sizeof(IDSP_INFO_s));

        if (dump_level >= 0) { // 0

            amba_ik_system_print_uint32_5("[IK] [C2Y]No.%d", IdspInfo.C2Y.CapSeqNo, DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_uint32_5("[IK] sec2_luma_buf %d x %d (%d): 0x%08x", IdspInfo.C2Y.Sec2R2y.LumaWidth, IdspInfo.C2Y.Sec2R2y.LumaHeight, IdspInfo.C2Y.Sec2R2y.LumaPitch, IdspInfo.C2Y.Sec2R2y.LumaAddr, DC_I);
            amba_ik_system_print_uint32_5("[IK] sec2_chroma_buf %d x %d (%d): 0x%08x", IdspInfo.C2Y.Sec2R2y.ChromaWidth, IdspInfo.C2Y.Sec2R2y.ChromaHeight, IdspInfo.C2Y.Sec2R2y.ChromaPitch, IdspInfo.C2Y.Sec2R2y.ChromaAddr, DC_I);

            amba_ik_system_print_uint32_5("[IK] [MCTF]No.%d", IdspInfo.vWarpMctf.CapSeqNo, DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_uint32_5("[IK] mctf_luma_buf: %d x %d (%d): 0x%08x", IdspInfo.vWarpMctf.Mctf.LumaWidth, IdspInfo.vWarpMctf.Mctf.LumaHeight, IdspInfo.vWarpMctf.Mctf.LumaPitch, IdspInfo.vWarpMctf.Mctf.LumaAddr, DC_I);
            amba_ik_system_print_uint32_5("[IK] mctf_chroma_buf: %d x %d (%d): 0x%08x", IdspInfo.vWarpMctf.Mctf.ChromaWidth, IdspInfo.vWarpMctf.Mctf.ChromaHeight, IdspInfo.vWarpMctf.Mctf.ChromaPitch, IdspInfo.vWarpMctf.Mctf.ChromaAddr, DC_I);
            amba_ik_system_print_uint32_5("[IK] mcts_luma_buf: %d x %d (%d): 0x%08x", IdspInfo.vWarpMctf.Mcts.LumaWidth, IdspInfo.vWarpMctf.Mcts.LumaHeight, IdspInfo.vWarpMctf.Mcts.LumaPitch, IdspInfo.vWarpMctf.Mcts.LumaAddr, DC_I);
            amba_ik_system_print_uint32_5("[IK] mcts_chroma_buf: %d x %d (%d): 0x%08x", IdspInfo.vWarpMctf.Mcts.ChromaWidth, IdspInfo.vWarpMctf.Mcts.ChromaHeight, IdspInfo.vWarpMctf.Mcts.ChromaPitch, IdspInfo.vWarpMctf.Mcts.ChromaAddr, DC_I);
        }

        if (dump_level = 99999) { // 1, dump config
            //sec2_cfg
            if(c2y->sec2_cfg_addr != 0) {
                DumpSize = CV1_MAX_IDSP_CFG;//, c2y->sec2_Cfg_len);
                (void)amba_ik_system_sprint(FileName, "%s\\%d_sec2_cfg.bin", dump_folder, c2y->cap_seq_no);
                Fid = AmbaFS_fopen(FileName, Fmode);
                if (Fid == NULL ) {
                    amba_ik_system_print("[IK] File open fail. Skip dumping debug data %s", FileName);
                    return Rval;
                }
                pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)c2y->sec2_cfg_addr);
                AmbaCache_Invalidate(pDumpAddress, DumpSize);//, c2y->sec2_Cfg_len);
                Rval = AmbaFS_fwrite((void const*) pDumpAddress, sizeof(UINT8), DumpSize, Fid);
                AmbaFS_fclose(Fid);
                amba_ik_system_print("[IK] dump c2y->sec2_cfg_addr = %x", c2y->sec2_cfg_addr);
            } else {
                amba_ik_system_print("[IK] dump fail, c2y->sec2_cfg_addr = %x", c2y->sec2_cfg_addr);
            }

            //sec3_cfg
            if(iav_ext_data.ext_cr_addr[0] != 0) {
                DumpSize = CV1_MAX_IDSP_CFG;//(512 * 50);
                (void)amba_ik_system_sprint(FileName, "%s\\%d_sec3_cfg.bin", dump_folder, status->cap_seq_no);
                Fid = AmbaFS_fopen(FileName, Fmode);
                if (Fid == NULL ) {
                    amba_ik_system_print("[IK] File open fail. Skip dumping debug data %s", FileName);
                    return Rval;
                }
                pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)iav_ext_data.ext_cr_addr[0]);
                AmbaCache_Invalidate(pDumpAddress, DumpSize);
                Rval = AmbaFS_fwrite((void const*) pDumpAddress, sizeof(UINT8), DumpSize, Fid);
                AmbaFS_fclose(Fid);
            } else {
                amba_ik_system_print("[IK] dump fail, sec3_cfg_addr = %x", iav_ext_data.ext_cr_addr[0]);
            }

            //mctf_cfg
            if(mctf_cfg_addr != 0) {
                DumpSize = 10064;//hdr->sec16_cfg_len
                (void)amba_ik_system_sprint(FileName, "%s\\%d_mctf_cfg.bin", dump_folder, c2y->cap_seq_no);
                Fid = AmbaFS_fopen(FileName, Fmode);
                if (Fid == NULL ) {
                    amba_ik_system_print("[IK] File open fail. Skip dumping debug data %s", FileName);
                    return Rval;
                }
                pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)mctf_cfg_addr);
                AmbaCache_Invalidate(pDumpAddress, DumpSize);
                Rval = AmbaFS_fwrite((void const*) pDumpAddress, sizeof(UINT8), DumpSize, Fid);
                AmbaFS_fclose(Fid);
            } else {
                amba_ik_system_print("[IK] dump fail, mctf_cfg_addr = %x", hdr->sec16_cfg_addr);
            }

            //mcts_cfg
            if(mcts_cfg_addr != 0) {
                DumpSize = 12752;//hdr->sec16_cfg_len
                (void)amba_ik_system_sprint(FileName, "%s\\%d_mcts_cfg.bin", dump_folder, c2y->cap_seq_no);
                Fid = AmbaFS_fopen(FileName, Fmode);
                if (Fid == NULL ) {
                    amba_ik_system_print("[IK] File open fail. Skip dumping debug data %s", FileName);
                    return Rval;
                }
                pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)mcts_cfg_addr);
                AmbaCache_Invalidate(pDumpAddress, DumpSize);
                Rval = AmbaFS_fwrite((void const*) pDumpAddress, sizeof(UINT8), DumpSize, Fid);
                AmbaFS_fclose(Fid);
            } else {
                amba_ik_system_print("[IK] dump fail, mcts_cfg_addr = %x", hdr->sec16_cfg_addr);
            }

#if 1
            if (Ability.VideoPipe != AMBA_IK_VIDEO_LINEAR) {
                //sec5_cfg
                if(iav_ext_data.ext_cr_addr[1] != 0) {
                    DumpSize = CV1_MAX_IDSP_CFG;//(512 * 7);
                    (void)amba_ik_system_sprint(FileName, "%s\\%d_sec5_cfg.bin", dump_folder, status->cap_seq_no);
                    Fid = AmbaFS_fopen(FileName, Fmode);
                    if (Fid == NULL ) {
                        amba_ik_system_print("[IK] File open fail. Skip dumping debug data %s", FileName);
                        return Rval;
                    }
                    pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)iav_ext_data.ext_cr_addr[1]);
                    AmbaCache_Invalidate(pDumpAddress, DumpSize);
                    Rval = AmbaFS_fwrite((void const*) pDumpAddress, sizeof(UINT8), DumpSize, Fid);
                    AmbaFS_fclose(Fid);
                } else {
                    amba_ik_system_print("[IK] dump fail, sec5_cfg_addr = %x", iav_ext_data.ext_cr_addr[1]);
                }

                //sec6_cfg
                if(iav_ext_data.ext_cr_addr[2] != 0) {
                    DumpSize = CV1_MAX_IDSP_CFG;//(512 * 2);
                    (void)amba_ik_system_sprint(FileName, "%s\\%d_sec6_cfg.bin", dump_folder, status->cap_seq_no);
                    Fid = AmbaFS_fopen(FileName, Fmode);
                    if (Fid == NULL ) {
                        amba_ik_system_print("[IK] File open fail. Skip dumping debug data %s", FileName);
                        return Rval;
                    }
                    pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)iav_ext_data.ext_cr_addr[2]);
                    AmbaCache_Invalidate(pDumpAddress, DumpSize);
                    Rval = AmbaFS_fwrite((void const*) pDumpAddress, sizeof(UINT8), DumpSize, Fid);
                    AmbaFS_fclose(Fid);
                } else {
                    amba_ik_system_print("[IK] dump fail, sec6_cfg_addr = %x", iav_ext_data.ext_cr_addr[2]);
                }

                //sec7_cfg
                if(iav_ext_data.ext_cr_addr[3] != 0) {
                    DumpSize = CV1_MAX_IDSP_CFG;//(512 * 3);
                    (void)amba_ik_system_sprint(FileName, "%s\\%d_sec7_cfg.bin", dump_folder, status->cap_seq_no);
                    Fid = AmbaFS_fopen(FileName, Fmode);
                    if (Fid == NULL ) {
                        amba_ik_system_print("[IK] File open fail. Skip dumping debug data %s", FileName);
                        return Rval;
                    }
                    pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)iav_ext_data.ext_cr_addr[3]);
                    AmbaCache_Invalidate(pDumpAddress, DumpSize);
                    Rval = AmbaFS_fwrite((void const*) pDumpAddress, sizeof(UINT8), DumpSize, Fid);
                    AmbaFS_fclose(Fid);
                } else {
                    amba_ik_system_print("[IK] dump fail, sec7_cfg_addr = %x", iav_ext_data.ext_cr_addr[3]);
                }

                //sec9_cfg
                if(hdr->sec9_cfg_addr != 0) {
                    DumpSize = CV1_MAX_IDSP_CFG;//hdr->sec9_cfg_len
                    (void)amba_ik_system_sprint(FileName, "%s\\%d_sec9_cfg.bin", dump_folder, hdr->cap_seq_no);
                    Fid = AmbaFS_fopen(FileName, Fmode);
                    if (Fid == NULL ) {
                        amba_ik_system_print("[IK] File open fail. Skip dumping debug data %s", FileName);
                        return Rval;
                    }
                    pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)hdr->sec9_cfg_addr);
                    AmbaCache_Invalidate(pDumpAddress, DumpSize);
                    Rval = AmbaFS_fwrite((void const*) pDumpAddress, sizeof(UINT8), DumpSize, Fid);
                    AmbaFS_fclose(Fid);
                } else {
                    amba_ik_system_print("[IK] dump fail, hdr->sec9_cfg_addr = %x", hdr->sec9_cfg_addr);
                }

                //sec10_cfg
                if(hdr->sec10_cfg_addr != 0) {
                    DumpSize = CV1_MAX_IDSP_CFG;//hdr->sec10_cfg_len
                    (void)amba_ik_system_sprint(FileName, "%s\\%d_sec10_cfg.bin", dump_folder, hdr->cap_seq_no);
                    Fid = AmbaFS_fopen(FileName, Fmode);
                    if (Fid == NULL ) {
                        amba_ik_system_print("[IK] File open fail. Skip dumping debug data %s", FileName);
                        return Rval;
                    }
                    pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)hdr->sec10_cfg_addr);
                    AmbaCache_Invalidate(pDumpAddress, DumpSize);
                    Rval = AmbaFS_fwrite((void const*) pDumpAddress, sizeof(UINT8), DumpSize, Fid);
                    AmbaFS_fclose(Fid);
                } else {
                    amba_ik_system_print("[IK] dump fail, hdr->sec10_cfg_addr = %x", hdr->sec10_cfg_addr);
                }

                //sec14_cfg
                if(hdr->sec14_cfg_addr != 0) {
                    DumpSize = CV1_MAX_IDSP_CFG;//hdr->sec14_cfg_len
                    (void)amba_ik_system_sprint(FileName, "%s\\%d_sec14_cfg.bin", dump_folder, hdr->cap_seq_no);
                    Fid = AmbaFS_fopen(FileName, Fmode);
                    if (Fid == NULL ) {
                        amba_ik_system_print("[IK] File open fail. Skip dumping debug data %s", FileName);
                        return Rval;
                    }
                    pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)hdr->sec14_cfg_addr);
                    AmbaCache_Invalidate(pDumpAddress, DumpSize);
                    Rval = AmbaFS_fwrite((void const*) pDumpAddress, sizeof(UINT8), DumpSize, Fid);
                    AmbaFS_fclose(Fid);
                } else {
                    amba_ik_system_print("[IK] dump fail, hdr->sec14_cfg_addr = %x", hdr->sec14_cfg_addr);
                }

                //sec15_cfg
                if(hdr->sec15_cfg_addr != 0) {
                    DumpSize = CV1_MAX_IDSP_CFG;//hdr->sec15_cfg_len
                    (void)amba_ik_system_sprint(FileName, "%s\\%d_sec15_cfg.bin", dump_folder, hdr->cap_seq_no);
                    Fid = AmbaFS_fopen(FileName, Fmode);
                    if (Fid == NULL ) {
                        amba_ik_system_print("[IK] File open fail. Skip dumping debug data %s", FileName);
                        return Rval;
                    }
                    pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)hdr->sec15_cfg_addr);
                    AmbaCache_Invalidate(pDumpAddress, DumpSize);
                    Rval = AmbaFS_fwrite((void const*) pDumpAddress, sizeof(UINT8), DumpSize, Fid);
                    AmbaFS_fclose(Fid);
                } else {
                    amba_ik_system_print("[IK] dump fail, hdr->sec15_cfg_addr = %x", hdr->sec15_cfg_addr);
                }

                //sec16_cfg
                if(hdr->sec16_cfg_addr != 0) {
                    DumpSize = CV1_MAX_IDSP_CFG;//hdr->sec16_cfg_len
                    (void)amba_ik_system_sprint(FileName, "%s\\%d_sec16_cfg.bin", dump_folder, hdr->cap_seq_no);
                    Fid = AmbaFS_fopen(FileName, Fmode);
                    if (Fid == NULL ) {
                        amba_ik_system_print("[IK] File open fail. Skip dumping debug data %s", FileName);
                        return Rval;
                    }
                    pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)hdr->sec16_cfg_addr);
                    AmbaCache_Invalidate(pDumpAddress, DumpSize);
                    Rval = AmbaFS_fwrite((void const*) pDumpAddress, sizeof(UINT8), DumpSize, Fid);
                    AmbaFS_fclose(Fid);
                } else {
                    amba_ik_system_print("[IK] dump fail, hdr->sec16_cfg_addr = %x", hdr->sec16_cfg_addr);
                }


            }
#endif
        }
        if (dump_level > 1 ) { // 2, dump internal buffer
            //sec2_luma
            if(c2y->luma_buf_addr != 0) {
                pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)c2y->luma_buf_addr);
                (void)amba_ik_system_sprint(FileName, "%s\\sec2_luma_%d_%d_%d.y", dump_folder, c2y->cap_seq_no, c2y->luma_buf_width, c2y->luma_buf_height);
                Rval |= IK_DumpCropedBuffer(pDumpAddress, FileName, c2y->luma_buf_pitch, c2y->luma_buf_width, c2y->luma_buf_height, p_buffer);
            } else {
                amba_ik_system_print("[IK] dump fail, sec2->luma_buf_addr = %x", c2y->luma_buf_addr);
            }
            //sec2_chroma
            if(c2y->chroma_buf_addr != 0) {
                pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)c2y->chroma_buf_addr);
                (void)amba_ik_system_sprint(FileName, "%s\\sec2_chroma_%d_%d_%d.uv", dump_folder, c2y->cap_seq_no, c2y->chroma_buf_width, c2y->chroma_buf_height);
                Rval |= IK_DumpCropedBuffer(pDumpAddress, FileName, c2y->chroma_buf_pitch, c2y->chroma_buf_width, c2y->chroma_buf_height, p_buffer);
            } else {
                amba_ik_system_print("[IK] dump fail, sec2->chroma_buf_addr = %x", c2y->chroma_buf_addr);
            }

            //sec3_luma
            if(status->luma_main_img_addr != 0) {
                pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)status->luma_main_img_addr);
                (void)amba_ik_system_sprint(FileName, "%s\\sec3_luma_buf_%d_%d_%d.y", dump_folder, status->cap_seq_no, status->luma_main_img_width, status->luma_main_img_height);
                Rval |= IK_DumpCropedBuffer(pDumpAddress, FileName, status->luma_main_img_pitch, status->luma_main_img_width, status->luma_main_img_height, p_buffer);
            } else {
                amba_ik_system_print("[IK] dump fail, status->sec3_luma_buf = %x", status->luma_main_img_addr);
            }
            //sec3_chroma
            if(status->chroma_main_img_addr != 0) {
                pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)status->chroma_main_img_addr);
                (void)amba_ik_system_sprint(FileName, "%s\\sec3_chroma_buf_%d_%d_%d.uv", dump_folder, status->cap_seq_no, status->chroma_main_img_width, status->chroma_main_img_height);
                Rval |= IK_DumpCropedBuffer(pDumpAddress, FileName, status->chroma_main_img_pitch, status->chroma_main_img_width, status->chroma_main_img_height, p_buffer);
            } else {
                amba_ik_system_print("[IK] dump fail, status->sec3_chroma_buf = %x", status->chroma_main_img_addr);
            }

            //Mctf_luma
            if(MctfStatus->luma_main_img_addr != 0) {
                pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)MctfStatus->luma_main_img_addr);
                (void)amba_ik_system_sprint(FileName, "%s\\mctf_luma_buf_%d_%d_%d.y", dump_folder, MctfStatus->cap_seq_no, MctfStatus->luma_main_img_width, MctfStatus->luma_main_img_height);
                Rval |= IK_DumpCropedBuffer(pDumpAddress, FileName, MctfStatus->luma_main_img_pitch, MctfStatus->luma_main_img_width, MctfStatus->luma_main_img_height, p_buffer);
            } else {
                amba_ik_system_print("[IK] dump fail, status->mctf_luma_buf = %x", MctfStatus->luma_main_img_addr);
            }
            //Mctf_chroma
            if(MctfStatus->chroma_main_img_addr != 0) {
                pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)MctfStatus->chroma_main_img_addr);
                (void)amba_ik_system_sprint(FileName, "%s\\mctf_chroma_buf_%d_%d_%d.uv", dump_folder, MctfStatus->cap_seq_no, MctfStatus->chroma_main_img_width, MctfStatus->chroma_main_img_height);
                Rval |= IK_DumpCropedBuffer(pDumpAddress, FileName, MctfStatus->chroma_main_img_pitch, MctfStatus->chroma_main_img_width, MctfStatus->chroma_main_img_height, p_buffer);
            } else {
                amba_ik_system_print("[IK] dump fail, status->mctf_chroma_buf = %x", MctfStatus->chroma_main_img_addr);
            }

            //Mcts_luma
            if(MctsStatus->luma_main_img_addr != 0) {
                pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)MctsStatus->luma_main_img_addr);
                (void)amba_ik_system_sprint(FileName, "%s\\mcts_luma_buf_%d_%d_%d.y", dump_folder, MctsStatus->cap_seq_no, MctsStatus->luma_main_img_width, MctsStatus->luma_main_img_height);
                Rval |= IK_DumpCropedBuffer(pDumpAddress, FileName, MctsStatus->luma_main_img_pitch, MctsStatus->luma_main_img_width, MctsStatus->luma_main_img_height, p_buffer);
            } else {
                amba_ik_system_print("[IK] dump fail, status->mcts_luma_buf = %x", MctsStatus->luma_main_img_addr);
            }
            //Mcts_chroma
            if(MctsStatus->chroma_main_img_addr != 0) {
                pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)MctsStatus->chroma_main_img_addr);
                (void)amba_ik_system_sprint(FileName, "%s\\mcts_chroma_buf_%d_%d_%d.uv", dump_folder, MctsStatus->cap_seq_no, MctsStatus->chroma_main_img_width, MctsStatus->chroma_main_img_height);
                Rval |= IK_DumpCropedBuffer(pDumpAddress, FileName, MctsStatus->chroma_main_img_pitch, MctsStatus->chroma_main_img_width, MctsStatus->chroma_main_img_height, p_buffer);
            } else {
                amba_ik_system_print("[IK] dump fail, status->mcts_chroma_buf = %x", MctsStatus->chroma_main_img_addr);
            }

            //PrevB_luma
            if(PrevBStatus->luma_main_img_addr != 0) {
                pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)PrevBStatus->luma_main_img_addr);
                (void)amba_ik_system_sprint(FileName, "%s\\PrevBStatus_luma_buf_%d_%d_%d.y", dump_folder, PrevBStatus->cap_seq_no, PrevBStatus->luma_main_img_width, PrevBStatus->luma_main_img_height);
                Rval |= IK_DumpCropedBuffer(pDumpAddress, FileName, PrevBStatus->luma_main_img_pitch, PrevBStatus->luma_main_img_width, PrevBStatus->luma_main_img_height, p_buffer);
            } else {
                amba_ik_system_print("[IK] dump fail, status->PrevBStatus_luma_buf = %x", PrevBStatus->luma_main_img_addr);
            }
            //PrevB_chroma
            if(PrevBStatus->chroma_main_img_addr != 0) {
                pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)PrevBStatus->chroma_main_img_addr);
                (void)amba_ik_system_sprint(FileName, "%s\\PrevBStatus_chroma_buf_%d_%d_%d.uv", dump_folder, PrevBStatus->cap_seq_no, PrevBStatus->chroma_main_img_width, PrevBStatus->chroma_main_img_height);
                Rval |= IK_DumpCropedBuffer(pDumpAddress, FileName, PrevBStatus->chroma_main_img_pitch, PrevBStatus->chroma_main_img_width, PrevBStatus->chroma_main_img_height, p_buffer);
            } else {
                amba_ik_system_print("[IK] dump fail, status->PrevBStatus_chroma_buf = %x", PrevBStatus->chroma_main_img_addr);
            }

#if 1

            if (Ability.VideoPipe != AMBA_IK_VIDEO_LINEAR) {
                //preblend_sec9
                if(hdr->preblend_sec9_buf_addr != 0) {
                    pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)hdr->preblend_sec9_buf_addr);
                    (void)amba_ik_system_sprint(FileName, "%s\\preblend_sec9_%d_%d_%d.y", dump_folder, hdr->cap_seq_no, hdr->preblend_sec9_buf_width, hdr->preblend_sec9_buf_height);
                    Rval |= IK_DumpCropedBuffer(pDumpAddress, FileName, hdr->preblend_sec9_buf_pitch, hdr->preblend_sec9_buf_width, hdr->preblend_sec9_buf_height, p_buffer);
                } else {
                    amba_ik_system_print("[IK] dump fail, hdr->preblend_sec9_buf_addr = %x", hdr->preblend_sec9_buf_addr);
                }
                //preblend_sec14
                if(hdr->preblend_sec14_buf_addr != 0) {
                    pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)hdr->preblend_sec14_buf_addr);
                    (void)amba_ik_system_sprint(FileName, "%s\\preblend_sec14_%d_%d_%d.y", dump_folder, hdr->cap_seq_no, hdr->preblend_sec14_buf_width, hdr->preblend_sec14_buf_height);
                    Rval |= IK_DumpCropedBuffer(pDumpAddress, FileName, hdr->preblend_sec14_buf_pitch, hdr->preblend_sec14_buf_width, hdr->preblend_sec14_buf_height, p_buffer);
                } else {
                    amba_ik_system_print("[IK] dump fail, hdr->preblend_sec14_buf_addr = %x", hdr->preblend_sec14_buf_addr);
                }
                //blend_sec10
                if(hdr->blend_sec10_buf_addr != 0) {
                    pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)hdr->blend_sec10_buf_addr);
                    (void)amba_ik_system_sprint(FileName, "%s\\blend_sec10_%d_%d_%d.y", dump_folder, hdr->cap_seq_no, hdr->blend_sec10_buf_width, hdr->blend_sec10_buf_height);
                    Rval |= IK_DumpCropedBuffer(pDumpAddress, FileName, hdr->blend_sec10_buf_pitch, hdr->blend_sec10_buf_width, hdr->blend_sec10_buf_height, p_buffer);
                } else {
                    amba_ik_system_print("[IK] dump fail, hdr->blend_sec10_buf_addr = %x", hdr->blend_sec10_buf_addr);
                }
                //blend_sec15
                if(hdr->blend_sec15_buf_addr != 0) {
                    pDumpAddress = (UINT8 *) AmbaMmuPhysToVirt((void *)hdr->blend_sec15_buf_addr);
                    (void)amba_ik_system_sprint(FileName, "%s\\blend_sec15_%d_%d_%d.y", dump_folder, hdr->cap_seq_no, hdr->blend_sec15_buf_width, hdr->blend_sec15_buf_height);
                    Rval |= IK_DumpCropedBuffer(pDumpAddress, FileName, hdr->blend_sec15_buf_pitch, hdr->blend_sec15_buf_width, hdr->blend_sec15_buf_height, p_buffer);
                } else {
                    amba_ik_system_print("[IK] dump fail, hdr->blend_sec15_buf_addr = %x", hdr->blend_sec15_buf_addr);
                }
            }
#endif
        }
    }

    return Rval;
}


UINT32 IK_DebugStillDump(char *dump_folder, INT32 dump_level, UINT8 *p_buffer)
{
    return 0;//TBD
}


//UINT32  AmbaIK_DebugDump(const AMBA_IK_MODE_CFG_s *pMode, char *DumpFolder, INT32 DumpLevel, UINT8  *pBuffer) {

//    UINT32  Rval = IK_OK ;


//    Rval = IK_DebugVideoDump(pMode, DumpFolder, DumpLevel, pBuffer);

//    return Rval;

//}
#endif

