/**
 *  @FileName       :: AmbaFwProgHeader.S
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Firmware programmer header
 */

.text

.globl begin_bst_image
.globl end_bst_image

.globl begin_bld_image
.globl end_bld_image

.globl begin_pba_image
.globl end_pba_image

.globl begin_atf_image
.globl end_atf_image

.globl begin_ipl_image
.globl end_ipl_image

.globl begin_bld2_image
.globl end_bld2_image

.globl begin_sys_image
.globl end_sys_image

.globl begin_r52sys_image
.globl end_r52sys_image

.globl begin_dsp_image
.globl end_dsp_image

.globl begin_rom_image
.globl end_rom_image

.globl begin_lnx_image
.globl end_lnx_image
.globl begin_dtb_image
.globl end_dtb_image
.globl begin_rfs_image
.globl end_rfs_image

.globl begin_xen_image
.globl end_xen_image
.globl begin_xtb_image
.globl end_xtb_image
.globl begin_x0k_image
.globl end_x0k_image
.globl begin_x0d_image
.globl end_x0d_image

.align 8

#ifdef __LINK_BST__
begin_bst_image:	.word	__BEGIN_BST_IMAGE__
#else
begin_bst_image:	.word	0x0
#endif

#ifdef __LINK_BLD__
begin_bld_image:	.word	__BEGIN_BLD_IMAGE__
#else
begin_bld_image:	.word	0x0
#endif

#ifdef __LINK_PBA__
begin_pba_image:	.word	__BEGIN_PBA_IMAGE__
#else
begin_pba_image:	.word	0x0
#endif

#ifdef __LINK_ATF__
begin_atf_image:	.word	__BEGIN_ATF_IMAGE__
#else
begin_atf_image:	.word	0x0
#endif

#ifdef __LINK_IPL__
begin_ipl_image:	.word	__BEGIN_IPL_IMAGE__
#else
begin_ipl_image:	.word	0x0
#endif

#ifdef __LINK_BLD2__
begin_bld2_image:    .word   __BEGIN_BLD2_IMAGE__
#else
begin_bld2_image:    .word   0x0
#endif


#ifdef __LINK_SYS__
begin_sys_image:	.word	__BEGIN_SYS_IMAGE__
#else
begin_sys_image:	.word	0x0
#endif

#ifdef __LINK_R52SYS__
begin_r52sys_image:	.word	__BEGIN_R52SYS_IMAGE__
#else
begin_r52sys_image:	.word	0x0
#endif

#ifdef __LINK_XEN__
begin_xen_image:	.word	__BEGIN_XEN_IMAGE__
#else
begin_xen_image:	.word	0x0
#endif

#ifdef __LINK_XTB__
begin_xtb_image:	.word	__BEGIN_XTB_IMAGE__
#else
begin_xtb_image:	.word	0x0
#endif

#ifdef __LINK_X0K__
begin_x0k_image:	.word	__BEGIN_X0K_IMAGE__
#else
begin_x0k_image:	.word	0x0
#endif

#ifdef __LINK_X0D__
begin_x0d_image:	.word	__BEGIN_X0D_IMAGE__
#else
begin_x0d_image:	.word	0x0
#endif

#ifdef __LINK_ROM__
begin_rom_image:	.word	__BEGIN_ROM_IMAGE__
#else
begin_rom_image:	.word	0x0
#endif

#ifdef __LINK_DSP__
begin_dsp_image:	.word	__BEGIN_DSP_IMAGE__
#else
begin_dsp_image:	.word	0x0
#endif

#ifdef __LINK_LNX__
begin_lnx_image:	.word	__BEGIN_LNX_IMAGE__
#else
begin_lnx_image:	.word	0x0
#endif

#ifdef __LINK_DTB__
begin_dtb_image:	.word	__BEGIN_DTB_IMAGE__
#else
begin_dtb_image:	.word	0x0
#endif

#ifdef __LINK_RFS__
begin_rfs_image:	.word	__BEGIN_RFS_IMAGE__
#else
begin_rfs_image:	.word	0x0
#endif

// end image

#ifdef __LINK_BST__
end_bst_image:	.word	__END_BST_IMAGE__
#else
end_bst_image:	.word	0x0
#endif

#ifdef __LINK_BLD__
end_bld_image:	.word	__END_BLD_IMAGE__
#else
end_bld_image:	.word	0x0
#endif

#ifdef __LINK_PBA__
end_pba_image:	.word	__END_PBA_IMAGE__
#else
end_pba_image:	.word	0x0
#endif

#ifdef __LINK_ATF__
end_atf_image:	.word	__END_ATF_IMAGE__
#else
end_atf_image:	.word	0x0
#endif

#ifdef __LINK_IPL__
end_ipl_image:	.word	__END_IPL_IMAGE__
#else
end_ipl_image:	.word	0x0
#endif

#ifdef __LINK_BLD2__
end_bld2_image:  .word   __END_BLD2_IMAGE__
#else
end_bld2_image:  .word   0x0
#endif

#ifdef __LINK_SYS__
end_sys_image:	.word	__END_SYS_IMAGE__
#else
end_sys_image:	.word	0x0
#endif

#ifdef __LINK_R52SYS__
end_r52sys_image:	.word	__END_R52SYS_IMAGE__
#else
end_r52sys_image:	.word	0x0
#endif

#ifdef __LINK_XEN__
end_xen_image:	.word	__END_XEN_IMAGE__
#else
end_xen_image:	.word	0x0
#endif

#ifdef __LINK_XTB__
end_xtb_image:	.word	__END_XTB_IMAGE__
#else
end_xtb_image:	.word	0x0
#endif

#ifdef __LINK_X0K__
end_x0k_image:	.word	__END_X0K_IMAGE__
#else
end_x0k_image:	.word	0x0
#endif

#ifdef __LINK_X0D__
end_x0d_image:	.word	__END_X0D_IMAGE__
#else
end_x0d_image:	.word	0x0
#endif

#ifdef __LINK_ROM__
end_rom_image:	.word	__END_ROM_IMAGE__
#else
end_rom_image:	.word	0x0
#endif

#ifdef __LINK_DSP__
end_dsp_image:	.word	__END_DSP_IMAGE__
#else
end_dsp_image:	.word	0x0
#endif

#ifdef __LINK_LNX__
end_lnx_image:	.word	__END_LNX_IMAGE__
#else
end_lnx_image:	.word	0x0
#endif

#ifdef __LINK_DTB__
end_dtb_image:	.word	__END_DTB_IMAGE__
#else
end_dtb_image:	.word	0x0
#endif

#ifdef __LINK_RFS__
end_rfs_image:	.word	__END_RFS_IMAGE__
#else
end_rfs_image:	.word	0x0
#endif

