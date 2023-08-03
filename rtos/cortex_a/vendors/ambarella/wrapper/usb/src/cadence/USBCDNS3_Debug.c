/**
 *  @file USBCDNS3_Debug.c
 *
 *  Copyright (c) 2021 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details USB driver for Cadence USB device/host controller.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

static void PrintU32_1(CDNS3_PRINT_f PrintFunc, const char *FmtStr, UINT32 Arg1)
{
    UINT32 u32_args[2];
    static char str_buf[255];
    u32_args[0] = Arg1;
    (void)IO_UtilityStringPrintUInt32(str_buf, 255, FmtStr, 1, u32_args);
    if (PrintFunc != NULL) {
        PrintFunc(str_buf);
    }
}

static void PrintU32_5(CDNS3_PRINT_f PrintFunc, const char *FmtStr, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    UINT32 u32_args[5];
    static char str_buf[255];
    u32_args[0] = Arg1;
    u32_args[1] = Arg2;
    u32_args[2] = Arg3;
    u32_args[3] = Arg4;
    u32_args[4] = Arg5;
    (void)IO_UtilityStringPrintUInt32(str_buf, 255, FmtStr, 5, u32_args);
    if (PrintFunc != NULL) {
        PrintFunc(str_buf);
    }
}

static void PrintU64_1(CDNS3_PRINT_f PrintFunc, const char *FmtStr, UINT64 Arg1)
{
    UINT64 u64_args[2];
    static char str_buf[255];
    u64_args[0] = Arg1;
    (void)IO_UtilityStringPrintUInt64(str_buf, 255, FmtStr, 1, u64_args);
    if (PrintFunc != NULL) {
        PrintFunc(str_buf);
    }
}

static void PrintU64_5(CDNS3_PRINT_f PrintFunc, const char *FmtStr, UINT64 Arg1, UINT64 Arg2, UINT64 Arg3, UINT64 Arg4, UINT64 Arg5)
{
    UINT64 u64_args[5];
    static char str_buf[255];
    u64_args[0] = Arg1;
    u64_args[1] = Arg2;
    u64_args[2] = Arg3;
    u64_args[3] = Arg4;
    u64_args[4] = Arg5;
    (void)IO_UtilityStringPrintUInt64(str_buf, 255, FmtStr, 5, u64_args);
    PrintFunc(str_buf);
}


/**
 * Display capabilities
 * @param res driver resources
 */
static void ext_xhc_info_display(const XHCI_CAPABILITY_REG_s *CapabilityReg, CDNS3_PRINT_f PrintFunc)
{
    if ((CapabilityReg != NULL) && (PrintFunc != NULL)) {
        UINT32 value = CapabilityReg->LENGTH_VER;
        UINT32 scratchpad_value;

        PrintU64_1(PrintFunc, "XHCI Capability: 0x%X", IO_UtilityPtrToU64Addr(CapabilityReg));
        PrintU32_1(PrintFunc, "    CAP_VER: 0x%X", value);
        PrintU32_5(PrintFunc, "        CAPLENGTH: 0x%X, VERSION: 0x%X",
            USBCDNS3_U32BitsRead(XHCI_LENGTH_VER_CAPLENGTH_MASK, XHCI_LENGTH_VER_CAPLENGTH_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_LENGTH_VER_HCIVERSION_MASK, XHCI_LENGTH_VER_HCIVERSION_SHIFT, value),
            0, 0, 0);

        value = CapabilityReg->HCSPARAMS1;
        PrintU32_1(PrintFunc, "    HCSPARAMS1: 0x%X", value);
        PrintU32_5(PrintFunc, "        MaxSlots: %d, MaxIntrs: %d, MaxPorts: %d",
            USBCDNS3_U32BitsRead(XHCI_HCSPARAMS1_MAXSLOTS_MASK, XHCI_HCSPARAMS1_MAXSLOTS_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCSPARAMS1_MAXINTRS_MASK, XHCI_HCSPARAMS1_MAXINTRS_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCSPARAMS1_MAXPORTS_MASK, XHCI_HCSPARAMS1_MAXPORTS_SHIFT, value),
            0, 0);

        value = CapabilityReg->HCSPARAMS2;
        scratchpad_value = USBCDNS3_U32BitsRead(XHCI_HCSPARAMS2_MAXSPBUFLO_MASK, XHCI_HCSPARAMS2_MAXSPBUFLO_SHIFT, value);
        scratchpad_value += (USBCDNS3_U32BitsRead(XHCI_HCSPARAMS2_MAXSPBUFHI_MASK, XHCI_HCSPARAMS2_MAXSPBUFHI_SHIFT, value) << 5UL);

        PrintU32_1(PrintFunc, "    HCSPARAMS2: 0x%X", value);
        PrintU32_5(PrintFunc, "        IST: %d, ERSTMax: %d, SPR: %d, MaxSpdBuf: %d",
            USBCDNS3_U32BitsRead(XHCI_HCSPARAMS2_IST_MASK, XHCI_HCSPARAMS2_IST_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCSPARAMS2_ERSTMAX_MASK, XHCI_HCSPARAMS2_ERSTMAX_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCSPARAMS2_SPR_MASK, XHCI_HCSPARAMS2_SPR_SHIFT, value),
            scratchpad_value,
            0);

        value = CapabilityReg->HCSPARAMS3;
        PrintU32_1(PrintFunc, "    HCSPARAMS3: 0x%X", value);
        PrintU32_5(PrintFunc, "        U1DevExitLatency: %d, U2DevExitLatency: %d",
            USBCDNS3_U32BitsRead(XHCI_HCSPARAMS3_U1DEVEXITLAT_MASK, XHCI_HCSPARAMS3_U1DEVEXITLAT_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCSPARAMS3_U2DEVEXITLAT_MASK, XHCI_HCSPARAMS3_U2DEVEXITLAT_SHIFT, value),
            0, 0, 0);

        value = CapabilityReg->HCCPARAMS1;
        PrintU32_1(PrintFunc, "    HCCPARAMS1: 0x%X", value);
        PrintU32_5(PrintFunc, "        AC64: %d, BNC: %d, CSZ: %d, PPC: %d, PIND: %d",
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS1_AC64_MASK, XHCI_HCCPARAMS1_AC64_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS1_BNC_MASK, XHCI_HCCPARAMS1_BNC_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS1_CSZ_MASK, XHCI_HCCPARAMS1_CSZ_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS1_PPC_MASK, XHCI_HCCPARAMS1_PPC_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS1_PIND_MASK, XHCI_HCCPARAMS1_PIND_SHIFT, value));
        PrintU32_5(PrintFunc, "        LHRC: %d, LTC: %d, NSS: %d, PAE: %d, SPC: %d",
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS1_LHRC_MASK, XHCI_HCCPARAMS1_LHRC_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS1_LTC_MASK, XHCI_HCCPARAMS1_LTC_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS1_NSS_MASK, XHCI_HCCPARAMS1_NSS_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS1_PAE_MASK, XHCI_HCCPARAMS1_PAE_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS1_SPC_MASK, XHCI_HCCPARAMS1_SPC_SHIFT, value));
        PrintU32_5(PrintFunc, "        MaxPSASize: %d, xECP: 0x%X",
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS1_MAXPSASIZE_MASK, XHCI_HCCPARAMS1_MAXPSASIZE_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS1_XECP_MASK, XHCI_HCCPARAMS1_XECP_SHIFT, value),
            0, 0, 0);

        value = CapabilityReg->HCCPARAMS2;
        PrintU32_1(PrintFunc, "    HCCPARAMS2: 0x%X", value);
        PrintU32_5(PrintFunc, "        U3C: %d, CMC: %d, FSC: %d, CTC: %d, LEC: %d",
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS2_U3C_MASK, XHCI_HCCPARAMS2_U3C_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS2_CMC_MASK, XHCI_HCCPARAMS2_CMC_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS2_FSC_MASK, XHCI_HCCPARAMS2_FSC_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS2_CTC_MASK, XHCI_HCCPARAMS2_CTC_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS2_LEC_MASK, XHCI_HCCPARAMS2_LEC_SHIFT, value));
        PrintU32_5(PrintFunc, "        CIC: %d, ETC: %d, ETC_TSC: %d, GSC: %d, VTC: %d",
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS2_CIC_MASK, XHCI_HCCPARAMS2_CIC_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS2_ETC_MASK, XHCI_HCCPARAMS2_ETC_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS2_ETC_TSC_MASK, XHCI_HCCPARAMS2_ETC_TSC_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS2_GSC_MASK, XHCI_HCCPARAMS2_GSC_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_HCCPARAMS2_VTC_MASK, XHCI_HCCPARAMS2_VTC_SHIFT, value));

        PrintU32_1(PrintFunc, "    DBOFF: 0x%X", CapabilityReg->DBOFF);
        PrintU32_1(PrintFunc, "    RTSOFF: 0x%X", CapabilityReg->RTSOFF);
    }
}

static void ext_xhc_operation_reg_display(const XHCI_OPERATION_REG_s *OpReg, CDNS3_PRINT_f PrintFunc)
{
    if (OpReg != NULL) {
        UINT32 value;
        UINT32 i;
        //XHCI_RING_ELEMENT_s *trb;

        PrintU64_1(PrintFunc, "XHCI Operation: 0x%X", IO_UtilityPtrToU64Addr(OpReg));

        value = OpReg->USBCMD;
        PrintU32_5(PrintFunc, "    USBCMD: 0x%X", value, 0, 0, 0, 0);
        PrintU32_5(PrintFunc, "        R/S: %d, HCRST: %d, INTE: %d, HSEE: %d",
            USBCDNS3_U32BitsRead(XHCI_USBCMD_R_S_MASK, XHCI_USBCMD_R_S_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_USBCMD_HCRST_MASK, XHCI_USBCMD_HCRST_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_USBCMD_INTE_MASK, XHCI_USBCMD_INTE_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_USBCMD_HSEE_MASK, XHCI_USBCMD_HSEE_SHIFT, value),
            0);

        value = OpReg->USBSTS;
        PrintU32_5(PrintFunc, "    USBSTS: 0x%X", value, 0, 0, 0, 0);
        PrintU32_5(PrintFunc, "        HCH: %d, EINT: %d, PCD: %d, CNR: %d, HCE: %d",
            USBCDNS3_U32BitsRead(XHCI_USBSTS_HCH_MASK, XHCI_USBSTS_HCH_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_USBSTS_EINT_MASK, XHCI_USBSTS_EINT_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_USBSTS_PCD_MASK, XHCI_USBSTS_PCD_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_USBSTS_CNR_MASK, XHCI_USBSTS_CNR_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_USBSTS_HCE_MASK, XHCI_USBSTS_HCE_SHIFT, value));

        value = OpReg->PAGESIZE;
        PrintU32_5(PrintFunc, "    PAGESIZE: %d(0x%X)", value, value, 0, 0, 0);

#if 0
        value_lo = (UINT32)OpReg->CRCR;
        value_hi = (UINT32)(OpReg->CRCR >> 32UL);
        USB_UtilityPrintUInt5("    CRCR: 0x%08X%08X", value_hi, value_lo, 0, 0, 0);
        USB_UtilityPrintUInt5("        RCS: %d, CS: %d, CA: %d, CRR: %d, LO_PTR: 0x%X",
            USBCDNS3_U32BitsRead(XHCI_CRCR_LO_RCS_MASK, XHCI_CRCR_LO_RCS_SHIFT, value_lo),
            USBCDNS3_U32BitsRead(XHCI_CRCR_LO_CS_MASK, XHCI_CRCR_LO_CS_SHIFT, value_lo),
            USBCDNS3_U32BitsRead(XHCI_CRCR_LO_CA_MASK, XHCI_CRCR_LO_CA_SHIFT, value_lo),
            USBCDNS3_U32BitsRead(XHCI_CRCR_LO_CRR_MASK, XHCI_CRCR_LO_CRR_SHIFT, value_lo),
            USBCDNS3_U32BitsRead(XHCI_CRCR_LO_CRPTR_L_MASK, XHCI_CRCR_LO_CRPTR_L_SHIFT, value_lo));
        USB_UtilityPrintUInt5("        HI_PTR: 0x%X", value_hi, 0, 0, 0, 0);
        trb = USBCDNS3_U64AddrToTrbRingPtr(USBCDNS3_U32BitsRead(XHCI_CRCR_LO_CRPTR_L_MASK, XHCI_CRCR_LO_CRPTR_L_SHIFT, value_lo));
        for (i = 0; i < AMBA_XHCI_PRODUCER_QUEUE_SIZE; i++) {
            USB_UtilityPrintUInt5("            CMD TRB[%d]: 0x%X", i, IO_UtilityPtrToU32Addr(trb), 0, 0, 0);
            if (trb != NULL) {
                USB_UtilityPrintUInt5("                C: %d, Type: %d, Slot: %d, Code: %d, PTR 0x%X",
                                        USBCDNS3_TrbToggleBitGet(trb),
                                        USBCDNS3_TrbTypeGet(trb),
                                        USBCDNS3_TrbSlotIdGet(trb),
                                        USBCDNS3_TrbCmplCodeGet(trb),
                                        trb->DWord0);
            }
            trb++;
        }
#endif
        PrintU64_1(PrintFunc, "    DCBAAP: 0x%X", OpReg->DCBAAP);

        value = OpReg->CONFIG;
        PrintU32_1(PrintFunc, "    CONFIG: 0x%X", value);
        PrintU32_5(PrintFunc, "        MaxSlotsEn: %d, U3E: %d, CIE: %d",
            USBCDNS3_U32BitsRead(XHCI_CONFIG_MAXSLOTSEN_MASK, XHCI_CONFIG_MAXSLOTSEN_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_CONFIG_U3E_MASK, XHCI_CONFIG_U3E_SHIFT, value),
            USBCDNS3_U32BitsRead(XHCI_CONFIG_CIE_MASK, XHCI_CONFIG_CIE_SHIFT, value),
            0, 0);


        for (i = 0; i < 2UL; i++) {
            const XHCI_PORT_s *port_reg = &OpReg->PORTArray[i];
            PrintU32_1(PrintFunc, "    PORT[%d]", i);
            value = port_reg->PORTSC;
            PrintU32_1(PrintFunc, "        PORTSC: 0x%X", value);
            PrintU32_5(PrintFunc, "            CCS: %d, PED: %d, OCA: %d, PR: %d, PLS: %d",
                USBCDNS3_U32BitsRead(XHCI_PORTSC_CCS_MASK, XHCI_PORTSC_CCS_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_PED_MASK, XHCI_PORTSC_PED_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_OCA_MASK, XHCI_PORTSC_OCA_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_PR_MASK, XHCI_PORTSC_PR_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_PLS_MASK, XHCI_PORTSC_PLS_SHIFT, value));
            PrintU32_5(PrintFunc, "            PP: %d, SPEED: %d, LWS: %d, CSC: %d, PEC: %d",
                USBCDNS3_U32BitsRead(XHCI_PORTSC_PP_MASK, XHCI_PORTSC_PP_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_PORTSPEED_MASK, XHCI_PORTSC_PORTSPEED_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_LWS_MASK, XHCI_PORTSC_LWS_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_CSC_MASK, XHCI_PORTSC_CSC_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_PEC_MASK, XHCI_PORTSC_PEC_SHIFT, value));
            PrintU32_5(PrintFunc, "            WRC: %d, OCC: %d, PRC: %d, PLC: %d, CEC: %d",
                USBCDNS3_U32BitsRead(XHCI_PORTSC_WRC_MASK, XHCI_PORTSC_WRC_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_OCC_MASK, XHCI_PORTSC_OCC_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_PRC_MASK, XHCI_PORTSC_PR_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_PLC_MASK, XHCI_PORTSC_PLC_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_CEC_MASK, XHCI_PORTSC_CEC_SHIFT, value));
            PrintU32_5(PrintFunc, "            CAS: %d, WCE: %d, WDE: %d, DR: %d, WPR: %d",
                USBCDNS3_U32BitsRead(XHCI_PORTSC_CAS_MASK, XHCI_PORTSC_CAS_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_WCE_MASK, XHCI_PORTSC_WCE_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_WDE_MASK, XHCI_PORTSC_WDE_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_DR_MASK, XHCI_PORTSC_DR_SHIFT, value),
                USBCDNS3_U32BitsRead(XHCI_PORTSC_WPR_MASK, XHCI_PORTSC_WPR_SHIFT, value));

            value = port_reg->PORTPMSC;
            PrintU32_1(PrintFunc, "        PORTPMSC: 0x%X", value);

            value = port_reg->PORTLI;
            PrintU32_1(PrintFunc, "        PORTLI: 0x%X", value);

            value = port_reg->PORTHLPMC;
            PrintU32_1(PrintFunc, "        PORTHLPMC: 0x%X", value);
        }
    }
}

static void ext_xhc_runtime_reg_display(const XHCI_RUNTIME_REG_s *RtReg, CDNS3_PRINT_f PrintFunc)
{
    if (RtReg != NULL) {
        UINT32 value;
        UINT32 i;
        const UINT32 *ptr_u32;
        UINT32 rss;
        UINT32 trb_idx;
        UINT32 value_lo;
        const XHCI_RING_ELEMENT_s *trb;
        const void                *ptr_void;

        PrintU64_1(PrintFunc, "XHCI Runtime: 0x%X", IO_UtilityPtrToU64Addr(RtReg));

        value = RtReg->MFINDEX;
        PrintU32_1(PrintFunc, "    MFINDEX: 0x%X", value);

        for (i = 0; i < 1UL; i++) {
            const XHCI_INTERRUPT_REG_s *int_reg = &RtReg->INTArray[i];
            PrintU32_1(PrintFunc, "    INT[%d]", i);

            value = int_reg->IMAN;
            PrintU32_1(PrintFunc, "        IMAN: 0x%X", value);
            PrintU32_5(PrintFunc, "            IP: %d, IE: %d",
                            USBCDNS3_U32BitsRead(XHCI_IMAN0_IP_MASK, XHCI_IMAN0_IP_SHIFT, value),
                            USBCDNS3_U32BitsRead(XHCI_IMAN0_IE_MASK, XHCI_IMAN0_IE_SHIFT, value),
                            0, 0, 0);

            value = int_reg->IMOD;
            PrintU32_1(PrintFunc, "        IMOD: 0x%X", value);
            PrintU32_5(PrintFunc, "            IMODI: %d, IMODC: %d",
                            USBCDNS3_U32BitsRead(XHCI_IMOD0_IMODI_MASK, XHCI_IMOD0_IMODI_SHIFT, value),
                            USBCDNS3_U32BitsRead(XHCI_IMOD0_IMODC_MASK, XHCI_IMOD0_IMODC_SHIFT, value),
                            0, 0, 0);

            value = int_reg->ERSTSZ;
            PrintU32_1(PrintFunc, "        ERSTSZ: 0x%X", value);

            PrintU64_1(PrintFunc, "        ERSTBA: 0x%X", int_reg->ERSTBA);


            //ptr_u32 = (UINT32 *)IO_UtilityU64AddrToPtr(int_reg->ERSTBA);
            ptr_void = IO_UtilityU64AddrToPtr(int_reg->ERSTBA);
            AmbaMisra_TypeCast(&ptr_u32, &ptr_void);
            if (ptr_u32 != NULL) {
                rss = ptr_u32[2] & 0x0FFFFU;
                PrintU32_5(PrintFunc, "            Ring Segment Base Addr: 0x%08X%08X", ptr_u32[1], ptr_u32[0], 0, 0, 0);
                PrintU32_5(PrintFunc, "            Ring Segment Size: %d(0x%X)", rss, rss, 0, 0, 0);
                trb = USBCDNS3_U64AddrToTrbRingPtr((UINT64)ptr_u32[0]);
                for (trb_idx = 0; trb_idx < rss; trb_idx++) {
                    PrintU64_5(PrintFunc, "            EVENT TRB[%d]: 0x%X",
                               (UINT64)trb_idx,
                               IO_UtilityPtrToU64Addr(trb),
                               (UINT64)0U, (UINT64)0U, (UINT64)0U);
                    if (trb != NULL) {
                        PrintU32_5(PrintFunc, "                0x%08X, 0x%08X, 0x%08X, 0x%08X",
                                                trb->DWord0,
                                                trb->DWord1,
                                                trb->DWord2,
                                                trb->DWord3,
                                                0);
                        PrintU32_5(PrintFunc, "                C: %d, Type: %d, Slot: %d, Code: %d, PTR 0x%X",
                                                USBCDNS3_TrbToggleBitGet(trb),
                                                USBCDNS3_TrbTypeGet(trb),
                                                USBCDNS3_TrbSlotIdGet(trb),
                                                USBCDNS3_TrbCmplCodeGet(trb),
                                                trb->DWord0);
                    }
                    if (trb != NULL) {
                        trb++;
                    } else {
                        break;
                    }
                }
            }

            value_lo = (UINT32)int_reg->ERDP;
            PrintU64_1(PrintFunc, "        ERDP: 0x%", int_reg->ERDP);
            PrintU32_5(PrintFunc, "            DESI: %d, EHB: %d, LO_PTR: 0x%X",
                            USBCDNS3_U32BitsRead(XHCI_ERDP0_LO_DESI_MASK, XHCI_ERDP0_LO_DESI_SHIFT, value_lo),
                            USBCDNS3_U32BitsRead(XHCI_ERDP0_LO_EHB_MASK, XHCI_ERDP0_LO_EHB_SHIFT, value_lo),
                            USBCDNS3_U32BitsRead(XHCI_ERDP0_LO_ERDPTR_MASK, XHCI_ERDP0_LO_ERDPTR_SHIFT, value_lo),
                            0, 0);
        }
    }

}

/**
 * Function displays endpoints context
 * @param ctx pointer to input context
 * @param epOffset offset in input context where endpoint context starts
 */
void USBCDNS3_XHCInputContexEpDisplay(const XHCI_INPUT_CONTEXT_s *InputContext, UINT32 EpOffset, UINT32 DegbugFlag)
{
    const XHCI_INPUT_CONTEXT_s *ctx = InputContext;
    const UINT32               *ep_ctx = NULL;
    UINT32  dw;

    if (ctx != NULL) {

        if (EpOffset == XHCI_EP0_CONTEXT_OFFSET) {
            ep_ctx = ctx->Ep0Context;
        } else if ((EpOffset >= XHCI_EPX_CONTEXT_OFFSET) && (EpOffset < XHCI_EP_CONTEXT_MAX)) {
            ep_ctx = ctx->EpContext[EpOffset - XHCI_EPX_CONTEXT_OFFSET];
        } else {
            // no action
        }

        if (ep_ctx != NULL) {
            USB_UtilityPrintUInt5(DegbugFlag, "  EP Context[%d]:", EpOffset, 0, 0, 0, 0);
            dw = ep_ctx[0];
            USB_UtilityPrintUInt5(DegbugFlag, "    ESIT HI: 0x%X, Interval: %d, MaxPStream: %d, Mult: %d, EP State: %d",
                (dw >> 24U) & 0x0FFU,
                (dw >> 16U) & 0x0FFU,
                (dw >> 10U) & 0x01FU,
                (dw >> 8U)  &  0x03U,
                (dw)        &  0x07U);
            dw = ep_ctx[1];
            USB_UtilityPrintUInt5(DegbugFlag, "    MaxPacketSize: %d, MaxBurstSize: %d, HID: %d, Ep Type: %d, CErr: %d",
                (dw >> 16U) & 0x0FFFFU,
                (dw >> 8U)  &   0x0FFU,
                (dw >> 7U)  &    0x01U,
                (dw >> 3U)  &    0x07U,
                (dw >> 1U)  &    0x03U);
            dw = ep_ctx[2];
            USB_UtilityPrintUInt5(DegbugFlag, "    Dequeue Ptr LO: 0x%08X, DCS: %d",
                (dw >> 4U)  &  0x0FFFFFFFU,
                dw          &        0x01U,
                0, 0, 0);
            dw = ep_ctx[3];
            USB_UtilityPrintUInt5(DegbugFlag, "    Dequeue Ptr HI: 0x%08X", dw, 0, 0, 0, 0);
            dw = ep_ctx[4];
            USB_UtilityPrintUInt5(DegbugFlag, "    ESIT LO: 0x%X, Average TRB Length %d",
                (dw >> 16U) & 0x0FFFFU,
                (dw         & 0x0FFFFU),
                0, 0, 0);
        }
    }
}


void USBCDNS3_XHCInputContexDisplay(const XHCI_INPUT_CONTEXT_s *InputContext)
{
    UINT32 dw;
    UINT32 dw7;
    UINT32 degbug_flag = USB_PRINT_FLAG_L1;
    UINT32 i;
    UINT32 mask_a;

    // check if context is not NULL
    if (InputContext != NULL) {
        USB_UtilityPrintUInt64_1(degbug_flag,
            "INPUT_CONTEXT @ 0x%X",
            IO_UtilityPtrToU64Addr(InputContext));

        // display input control context
        mask_a = InputContext->InputControlContext[1];
        dw7 = InputContext->InputControlContext[7];
        USB_UtilityPrint(degbug_flag, "  Input Control Context:");
        USB_UtilityPrintUInt5(degbug_flag, "    D: 0x%08X", InputContext->InputControlContext[0], 0, 0, 0, 0);
        USB_UtilityPrintUInt5(degbug_flag, "    A: 0x%08X", InputContext->InputControlContext[1], 0, 0, 0, 0);
        USB_UtilityPrintUInt5(degbug_flag, "    ALT: 0x%X, INTF: 0x%X, COFNIG: 0x%X",
            (dw7 >> 16U) & 0x0FFU,
            (dw7 >> 8U) & 0x0FFU,
            (dw7) & 0x0FFU,
            0, 0);

        // display slot context
        USB_UtilityPrint(degbug_flag, "  Slot Context:");
        dw = InputContext->InputControlContext[0];
        USB_UtilityPrintUInt5(degbug_flag, "    Speed: %d, MTT: %d, Hub: %d, Context Entries: %d",
            (dw >> 20U) & 0x0FU,
            (dw >> 25U) & 0x01U,
            (dw >> 26U) & 0x01U,
            (dw >> 27U) & 0x1FU,
            0);
        dw = InputContext->InputControlContext[1];
        USB_UtilityPrintUInt5(degbug_flag, "    Number of Ports: %d, RH Port Number: %d, Max Exit Lantency: %d",
            (dw >> 24U) & 0x0FFU,
            (dw >> 16U) & 0x0FFU,
            (dw) & 0x0FFFFU,
            0,
            0);
        dw = InputContext->InputControlContext[2];
        USB_UtilityPrintUInt5(degbug_flag, "    Int Target: %d, TTT: %d, TTT Port Number: %d, TT Hub Slot ID: %d",
            (dw >> 22U) & 0x03FFU,
            (dw >> 16U) & 0x03U,
            (dw >> 8U) & 0x0FFU,
            dw & 0x0FFU,
            0);
        dw = InputContext->InputControlContext[3];
        USB_UtilityPrintUInt5(degbug_flag, "    Slot State: %d, Device Address: %d",
            (dw >> 27U) & 0x1FU,
            (dw >> 8U) & 0x0FFU,
            0, 0, 0);

        for (i = XHCI_EP0_CONTEXT_OFFSET; i < XHCI_EP_CONTEXT_MAX; i++) {

            UINT32 target = 1U;

            target = target << i;
            if ((target & mask_a) != 0U) {
                USBCDNS3_XHCInputContexEpDisplay(InputContext, i, degbug_flag);
            }
        }
    }

}

static void ext_xhc_input_context_ep_display(const XHCI_INPUT_CONTEXT_s *InputContext, UINT32 EpOffset, CDNS3_PRINT_f PrintFunc)
{
    const XHCI_INPUT_CONTEXT_s *ctx = InputContext;
    const UINT32               *ep_ctx = NULL;
    UINT32  dw;

    if (ctx != NULL) {

        if (EpOffset == XHCI_EP0_CONTEXT_OFFSET) {
            ep_ctx = ctx->Ep0Context;
        } else if ((EpOffset >= XHCI_EPX_CONTEXT_OFFSET) && (EpOffset < XHCI_EP_CONTEXT_MAX)) {
            ep_ctx = ctx->EpContext[EpOffset - XHCI_EPX_CONTEXT_OFFSET];
        } else {
            // no action
        }

        if (ep_ctx != NULL) {
            PrintU32_1(PrintFunc, "  EP Context[%d]:", EpOffset);
            dw = ep_ctx[0];
            PrintU32_5(PrintFunc, "    ESIT HI: 0x%X, Interval: %d, MaxPStream: %d, Mult: %d, EP State: %d",
                (dw >> 24U) & 0x0FFU,
                (dw >> 16U) & 0x0FFU,
                (dw >> 10U) & 0x01FU,
                (dw >> 8U)  &  0x03U,
                (dw)        &  0x07U);
            dw = ep_ctx[1];
            PrintU32_5(PrintFunc, "    MaxPacketSize: %d, MaxBurstSize: %d, HID: %d, Ep Type: %d, CErr: %d",
                (dw >> 16U) & 0x0FFFFU,
                (dw >> 8U)  &   0x0FFU,
                (dw >> 7U)  &    0x01U,
                (dw >> 3U)  &    0x07U,
                (dw >> 1U)  &    0x03U);
            dw = ep_ctx[2];
            PrintU32_5(PrintFunc, "    Dequeue Ptr LO: 0x%08X, DCS: %d",
                (dw >> 4U)  &  0x0FFFFFFFU,
                dw          &        0x01U,
                0, 0, 0);
            dw = ep_ctx[3];
            PrintU32_1(PrintFunc, "    Dequeue Ptr HI: 0x%08X", dw);
            dw = ep_ctx[4];
            PrintU32_5(PrintFunc, "    ESIT LO: 0x%X, Average TRB Length %d",
                (dw >> 16U) & 0x0FFFFU,
                (dw         & 0x0FFFFU),
                0, 0, 0);
        }
    }
}

static void ext_xhc_input_context_display(const XHCI_INPUT_CONTEXT_s *InputContext, CDNS3_PRINT_f PrintFunc)
{
    UINT32 dw;
    UINT32 dw7;
    UINT32 i;
    UINT32 mask_a;

    // check if context is not NULL
    if (InputContext != NULL) {
        PrintU64_1(PrintFunc, "INPUT_CONTEXT @ 0x%X",
            IO_UtilityPtrToU64Addr(InputContext));

        // display input control context
        mask_a = InputContext->InputControlContext[1];
        dw7 = InputContext->InputControlContext[7];
        PrintFunc("  Input Control Context:");
        PrintU32_1(PrintFunc, "    D: 0x%08X", InputContext->InputControlContext[0]);
        PrintU32_1(PrintFunc, "    A: 0x%08X", InputContext->InputControlContext[1]);
        PrintU32_5(PrintFunc, "    ALT: 0x%X, INTF: 0x%X, COFNIG: 0x%X",
            (dw7 >> 16U) & 0x0FFU,
            (dw7 >> 8U) & 0x0FFU,
            (dw7) & 0x0FFU,
            0, 0);

        // display slot context
        PrintFunc("  Slot Context:");
        dw = InputContext->InputControlContext[0];
        PrintU32_5(PrintFunc, "    Speed: %d, MTT: %d, Hub: %d, Context Entries: %d",
            (dw >> 20U) & 0x0FU,
            (dw >> 25U) & 0x01U,
            (dw >> 26U) & 0x01U,
            (dw >> 27U) & 0x1FU,
            0);
        dw = InputContext->InputControlContext[1];
        PrintU32_5(PrintFunc, "    Number of Ports: %d, RH Port Number: %d, Max Exit Lantency: %d",
            (dw >> 24U) & 0x0FFU,
            (dw >> 16U) & 0x0FFU,
            (dw) & 0x0FFFFU,
            0,
            0);
        dw = InputContext->InputControlContext[2];
        PrintU32_5(PrintFunc, "    Int Target: %d, TTT: %d, TTT Port Number: %d, TT Hub Slot ID: %d",
            (dw >> 22U) & 0x03FFU,
            (dw >> 16U) & 0x03U,
            (dw >> 8U) & 0x0FFU,
            dw & 0x0FFU,
            0);
        dw = InputContext->InputControlContext[3];
        PrintU32_5(PrintFunc, "    Slot State: %d, Device Address: %d",
            (dw >> 27U) & 0x1FU,
            (dw >> 8U) & 0x0FFU,
            0, 0, 0);

        for (i = XHCI_EP0_CONTEXT_OFFSET; i < XHCI_EP_CONTEXT_MAX; i++) {

            UINT32 target = 1U;

            target = target << i;
            if ((target & mask_a) != 0U) {
                ext_xhc_input_context_ep_display(InputContext, i, PrintFunc);
            }
        }
    }
}


void USBCDNS3_Regdump(CDNS3_PRINT_f PrintFunc)
{
    const XHC_CDN_DRV_RES_s *cdn_drv_res = USBCDNS3_DrvInstanceGet();

    ext_xhc_info_display(cdn_drv_res->Registers.xhciCapability, PrintFunc);
    ext_xhc_operation_reg_display(cdn_drv_res->Registers.xhciOperational, PrintFunc);
    ext_xhc_runtime_reg_display(cdn_drv_res->Registers.xhciRuntime, PrintFunc);
    ext_xhc_input_context_display(cdn_drv_res->InputContext, PrintFunc);
}


