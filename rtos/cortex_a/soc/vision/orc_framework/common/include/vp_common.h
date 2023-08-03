/*
 * Copyright (c) 2017-2018 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef VP_COMMON_H_FILE
#define VP_COMMON_H_FILE

#include <dsp_types.h>


/*!
  @file vp_utils.h

  @brief common structure and utilies to run VP dag

  This file defines data structures and functions for running a VP dag.
  It helps to aggregate the common utilities to run a VP dag, such as
  how to automatically load and initialize all the static resource 
  (filter coefficients, conv kernels, static tables and etc).

*/

typedef struct
{
  union
  {
    struct
    {
      uint32_t   width;
      uint32_t   height;
      uint32_t   depth;
      uint32_t   plane;
    };
    uint32_t dim[4];
  };
} vp_vect_t;

#define     VP_BUF_TYPE_VCB         0x00U
#define     VP_BUF_TYPE_VMB_SW      0x01U
#define     VP_BUF_TYPE_VMB_HW      0x02U

typedef struct
{
  uint32_t     dbase_in_dag;
  uint32_t     dbase_in_rc;
  uint32_t     desc_in_dag;
} vp_rc_dbuf_t;

typedef struct
{
  uint32_t          num_sw_dbufs;
  uint32_t          num_hw_dbufs;
  uint32_t          num_vbufs;
  uint32_t          org_vbase;
  uint32_t          org_dbase;
  vp_rc_dbuf_t      bufs;
} vp_static_rc_t;

#ifndef MISRA_COMPLIANCE

/*! @brief poke one specific dag field name into the run queue.

  @param _value  value to be poked
  @param _dagbase  dag base in vmem (byte address)
  @param _field field name to be poked, will generate 
                field_boffset and field_size from name
*/
#define vp_poke_dag_field(_dagbase,  _value, _field )                  \
do {                                                                   \
    coproc_poke_field((uint32_t)(_value),                              \
                          (_dagbase) + ((_field##_boffset) >> 5 << 2), \
                          ((_field##_boffset) & 31),                   \
                          (_field##_bsize));                           \
}while (0)

/*! @brief poke one specific dag field name into the load queue.

  @param _value  value to be poked
  @param _dagbase  dag base in vmem (byte address)
  @param _field field name to be poked, will generate 
                field_boffset and field_size from name
*/
#define vp_poke_dag_field_ldq(_dagbase,  _value, _field )              \
do {                                                                   \
    coproc_poke_field_ldq((uint32_t)(_value),                          \
                          (_dagbase) + ((_field##_boffset) >> 5 << 2), \
                          ((_field##_boffset) & 31),                   \
                          (_field##_bsize));                           \
}while (0)


/*! @brief poke one specific dag field name into the store queue.

  @param _value  value to be poked
  @param _dagbase  dag base in vmem (byte address)
  @param _field field name to be poked, will generate 
                field_boffset and field_size from name
*/
#define vp_poke_dag_field_stq(_dagbase,  _value, _field )              \
do {                                                                   \
    coproc_poke_field_stq((uint32_t)(_value),                          \
                          (_dagbase) + ((_field##_boffset) >> 5 << 2), \
                          ((_field##_boffset) & 31),                   \
                          (_field##_bsize));                           \
}while (0)

#endif /* !MISRA_COMPLIANCE */

/*! @brief common utility to load dag via cmem, break into 1K pieces

  @param dag_dbase     dram base adress of DAG
  @param dag_vbase     vmem base adress of DAG
  @param dag_size      dag size in bytes
  @param cmem_scratch  cmem scratch buffer (at least 1 K size)
*/
uint32_t vp_load_dag_via_cmem (uint32_t dag_dbase, uint32_t dag_vbase, uint32_t dag_size, uint32_t cmem_scratch);

/*! @brief vp_dummy_run
*/
#ifdef MISRA_COMPLIANCE
void vp_dummy_run(void);
#else /* !MISRA_COMPLIANCE */
#define vp_dummy_run() do { coproc_run (0); } while(0)
#endif /* ?MISRA_COMPLIANCE */


/*! @brief initialize vp to prepare for common tasks such load dag
*/
uint32_t vp_init (void);

/*! @brief common utility to load dag using software descriptor, need to call vp_init first

  @param dag_dbase     dram base adress of DAG
  @param dag_vbase     vmem base adress of DAG
  @param dag_size      dag size in bytes
  @param cmem_scratch  cmem scratch buffer (at least 1 K size)
*/
uint32_t vp_load_dag (uint32_t dag_dbase, uint32_t dag_vbase, uint32_t dag_size, uint32_t wait_needed);


/*! @brief common utility to change base address of all dram/vmem addresses

  @param dag_vbase     vmem base adress of DAG
  @param dag_dbase     dram base adress of DAG
  @param dbase         dram base for resource data
  @param vbase         vmem base for all vmem buffers
*/
uint32_t vp_process_dag_rc (uint32_t dag_vbase, uint32_t dag_dbase, uint32_t rc_dbase, uint32_t vbase); 


/*! @brief common utility to load all software managed resource files

  @param dag_vbase     vmem base adress of DAG
  @param dbase         dram base for resource data
*/
uint32_t vp_load_rc_sw_dbufs (uint32_t dag_vbase, uint32_t rc_dbase); 


/*! @brief      modify a value in vmem, utilizing dma between vmem/cmem
 *              need to trash one cmem word

  @param addr   vmem address of data to be changed
  @param offset offset to be applied
  @param vbase  cmem address to be trashed as intermediate buffer
*/
void vp_add_offset_in_vmem(uint32_t addr, uint32_t offset, uint32_t caddr);

/*! @brief generate scaling warp field

  @param sw     source image width
  @param sh     source image height
  @param dw     destination image width
  @param dh     destination image height
  @param grid   point to grid_spacing_log2 [0]: horizontal [1]: vertical; need to poke into DAG 
  @param field  point to generated warp field data
*/
uint32_t vp_gen_scaling_warp_field(uint32_t sw,
	uint32_t sh,
	uint32_t dw,
	uint32_t dh,
	uint32_t *grid_log2,
	uint32_t *field
	);


/*! @brief generate scaling warp field with initial phase

  @param sx     x start position in the source image, 4-bit fractional part
  @param sy     y start position in the source iamge, 4-bit fractional part
  @param ex     x end position in the source image, 4-bit fractional part
  @param ey     y end position in the source image, 4-bit fractional part
  @param dw     destination image width, integer position
  @param dh     destination image height, integer position
  @param grid   point to grid_spacing_log2 [0]: horizontal [1]: vertical; need to poke into DAG 
  @param field  point to generated warp field data
*/
uint32_t vp_gen_frac_scaling_warp_field(
	int32_t  sx,
	int32_t  sy,
	int32_t  ex,
	int32_t  ey,
	uint32_t dw,
	uint32_t dh,
	uint32_t *grid_log2,
	uint32_t *field
	);

uint32_t vp_check_reserved_vmem(void);
uint32_t vp_load_buf(uint32_t buf_dbase, uint32_t buf_vbase, uint32_t buf_size);
uint32_t vp_load_buf_nowait(uint32_t buf_dbase, uint32_t buf_vbase, uint32_t buf_size);
uint32_t vp_store_buf(uint32_t buf_dbase, uint32_t buf_vbase, uint32_t buf_size);
void vp_enable_hang_detect(uint32_t threshold);
uint32_t vp_hang_detect(void);
#ifdef CHIP_CV6
void visorc_set_preferred_dram_transfer_size(uint32_t val); // core_id is internally generated
#else /* !CHIP_CV6 */
void visorc_set_preferred_dram_transfer_size(uint32_t core_id, uint32_t val);
#endif /* ?CHIP_CV6 */

#ifdef CHIP_CV6
#define COPROC_STATUS_ADDR (0xF00000U)
#else /* !CHIP_CV6 */
#define COPROC_STATUS_ADDR (0x4U)
#endif /* ?CHIP_CV6 */
/*! @brief Decode the VP status register and generate error return codes

  @param status_register   peeked value at COPROC_STATUS_ADDR
*/
uint32_t vp_decode_status(uint32_t status_register);

#endif /* ?VP_COMMON_H_FILE */

