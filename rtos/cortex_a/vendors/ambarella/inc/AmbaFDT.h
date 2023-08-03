
#ifndef AMBAFDT_H
#define AMBAFDT_H

#if !defined(LIBFDT_H) && !defined(_LIBFDT_H)  //native start
#define FDT_FIRST_SUPPORTED_VERSION    0x02
#define FDT_LAST_SUPPORTED_VERSION    0x11
#define FDT_ERR_NOTFOUND    1
#define FDT_ERR_EXISTS        2
#define FDT_ERR_NOSPACE        3
#define FDT_ERR_BADOFFSET    4
#define FDT_ERR_BADPATH        5
#define FDT_ERR_BADPHANDLE    6
#define FDT_ERR_BADSTATE    7
#define FDT_ERR_TRUNCATED    8
#define FDT_ERR_BADMAGIC    9
#define FDT_ERR_BADVERSION    10
#define FDT_ERR_BADSTRUCTURE    11
#define FDT_ERR_BADLAYOUT    12
#define FDT_ERR_INTERNAL    13
#define FDT_ERR_BADNCELLS    14
#define FDT_ERR_BADVALUE    15
#define FDT_ERR_BADOVERLAY    16
#define FDT_ERR_NOPHANDLES    17
#define FDT_ERR_BADFLAGS    18
#define FDT_ERR_MAX        18
#define FDT_MAX_PHANDLE 0xfffffffeU
#define FDT_MAX_NCELLS        4
#define FDT_CREATE_FLAG_NO_NAME_DEDUP 0x1
#define FDT_CREATE_FLAGS_ALL    (FDT_CREATE_FLAG_NO_NAME_DEDUP)
#define FDT_MAGIC    0xd00dfeedU    /* 4: version, 4: total size */
#define FDT_TAGSIZE    sizeof(fdt32_t)
#define FDT_BEGIN_NODE    0x1        /* Start node: full name */
#define FDT_END_NODE    0x2        /* End node */
#define FDT_PROP    0x3        /* Property: name off, size, content */
#define FDT_NOP        0x4        /* nop */
#define FDT_END        0x9

#define FDT_V1_SIZE    (7*sizeof(fdt32_t))
#define FDT_V2_SIZE    (FDT_V1_SIZE + sizeof(fdt32_t))
#define FDT_V3_SIZE    (FDT_V2_SIZE + sizeof(fdt32_t))
#define FDT_V16_SIZE    FDT_V3_SIZE
#define FDT_V17_SIZE    (FDT_V16_SIZE + sizeof(fdt32_t))


typedef UINT16 fdt16_t;
typedef UINT32 fdt32_t;
typedef UINT64 fdt64_t;

/* WARNING: sizeof(struct fdt_property) is 4 bytes larger than native */
struct fdt_property {
    fdt32_t tag;
    fdt32_t len;
    fdt32_t nameoff;
    char data[4];
};

#if 0
struct fdt_header {
    fdt32_t magic;             /* magic word FDT_MAGIC */
    fdt32_t totalsize;         /* total size of DT block */
    fdt32_t off_dt_struct;         /* offset to structure */
    fdt32_t off_dt_strings;         /* offset to strings */
    fdt32_t off_mem_rsvmap;         /* offset to memory reserve map */
    fdt32_t version;         /* format version */
    fdt32_t last_comp_version;     /* last compatible version */

    /* version 2 fields below */
    fdt32_t boot_cpuid_phys;     /* Which physical CPU id we're
                        booting on */
    /* version 3 fields below */
    fdt32_t size_dt_strings;     /* size of the strings block */

    /* version 17 fields below */
    fdt32_t size_dt_struct;         /* size of the structure block */
};
#endif

static inline UINT32 fdt32_to_cpu(UINT32 x)
{
    return ((x & 0xffU) << 24) |
           ((x & 0xff00U) << 8) |
           ((x & 0xff0000U) >> 8) |
           ((x & 0xff000000U) >> 24);
}

static inline UINT64 fdt64_to_cpu(UINT64 x)
{
    return ((x & 0xffUL) << 56) |
           ((x & 0xff00UL) << 40) |
           ((x & 0xff0000UL) << 24) |
           ((x & 0xff000000UL) << 8) |
           ((x & 0xff00000000UL) >> 8) |
           ((x & 0xff0000000000UL) >> 24) |
           ((x & 0xff000000000000UL) >> 40) |
           ((x & 0xff00000000000000UL) >> 56);
}

const void *fdt_offset_ptr(const void *fdt, INT32 offset, UINT32 checklen);
UINT32 fdt_next_tag(const void *fdt, INT32 offset, INT32 *nextoffset);
INT32 fdt_next_node(const void *fdt, INT32 offset, INT32 *depth);
INT32 fdt_first_subnode(const void *fdt, INT32 offset);
INT32 fdt_next_subnode(const void *fdt, INT32 offset);
SIZE_t fdt_header_size(const void *fdt);
SIZE_t fdt_header_size_(UINT32 version);
INT32 fdt_check_header(const void *fdt);
INT32 fdt_move(const void *fdt, void *buf, INT32 bufsize);
INT32 fdt_check_full(const void *fdt, SIZE_t bufsize);
const char *fdt_get_string(const void *fdt, INT32 stroffset, INT32 *lenp);
const char *fdt_string(const void *fdt, INT32 stroffset);
INT32 fdt_find_max_phandle(const void *fdt, UINT32 *phandle);
INT32 fdt_generate_phandle(const void *fdt, UINT32 *phandle);
INT32 fdt_num_mem_rsv(const void *fdt);
INT32 fdt_get_mem_rsv(const void *fdt, INT32 n, UINT64 *address, UINT64 *size);
INT32 fdt_subnode_offset_namelen(const void *fdt, INT32 parentoffset, const char *name, INT32 namelen);
INT32 fdt_subnode_offset(const void *fdt, INT32 parentoffset, const char *name);
INT32 fdt_path_offset_namelen(const void *fdt, const char *path, INT32 namelen);
INT32 fdt_path_offset(const void *fdt, const char *path);
const char *fdt_get_name(const void *fdt, INT32 nodeoffset, INT32 *lenp);
INT32 fdt_first_property_offset(const void *fdt, INT32 nodeoffset);
INT32 fdt_next_property_offset(const void *fdt, INT32 offset);
const struct fdt_property *fdt_get_property_by_offset(const void *fdt, INT32 offset, INT32 *lenp);
const struct fdt_property *fdt_get_property_namelen(const void *fdt, INT32 nodeoffset, const char *name, INT32 namelen, INT32 *lenp);
const struct fdt_property *fdt_get_property(const void *fdt, INT32 nodeoffset, const char *name, INT32 *lenp);
const void *fdt_getprop_by_offset(const void *fdt, INT32 offset, const char **namep, INT32 *lenp);
const void *fdt_getprop_namelen(const void *fdt, INT32 nodeoffset, const char *name, INT32 namelen, INT32 *lenp);
const void *fdt_getprop(const void *fdt, INT32 nodeoffset, const char *name, INT32 *lenp);
UINT32 fdt_get_phandle(const void *fdt, INT32 nodeoffset);
const char *fdt_get_alias_namelen(const void *fdt, const char *name, INT32 namelen);
const char *fdt_get_alias(const void *fdt, const char *name);
INT32 fdt_get_path(const void *fdt, INT32 nodeoffset, char *buf, INT32 buflen);
INT32 fdt_supernode_atdepth_offset(const void *fdt, INT32 nodeoffset, INT32 supernodedepth, INT32 *nodedepth);
INT32 fdt_node_depth(const void *fdt, INT32 nodeoffset);
INT32 fdt_parent_offset(const void *fdt, INT32 nodeoffset);
INT32 fdt_node_offset_by_prop_value(const void *fdt, INT32 startoffset, const char *propname, const void *propval, INT32 proplen);
INT32 fdt_node_offset_by_phandle(const void *fdt, UINT32 phandle);
INT32 fdt_node_check_compatible(const void *fdt, INT32 nodeoffset, const char *compatible);
INT32 fdt_node_offset_by_compatible(const void *fdt, INT32 startoffset, const char *compatible);
INT32 fdt_stringlist_contains(const char *strlist, INT32 listlen, const char *str);
INT32 fdt_stringlist_count(const void *fdt, INT32 nodeoffset, const char *property);
INT32 fdt_stringlist_search(const void *fdt, INT32 nodeoffset, const char *property, const char *string);
const char *fdt_stringlist_get(const void *fdt, INT32 nodeoffset, const char *property, INT32 index, INT32 *lenp);
INT32 fdt_address_cells(const void *fdt, INT32 nodeoffset);
INT32 fdt_size_cells(const void *fdt, INT32 nodeoffset);
//INT32 fdt_setprop_inplace_namelen_partial(void *fdt, INT32 nodeoffset, const char *name, INT32 namelen, UINT32 idx, const void *val, INT32 len);
INT32 fdt_setprop_inplace(void *fdt, INT32 nodeoffset, const char *name, const void *val, INT32 len);
INT32 fdt_nop_property(void *fdt, INT32 nodeoffset, const char *name);
INT32 fdt_nop_node(void *fdt, INT32 nodeoffset);
INT32 fdt_create_with_flags(void *buf, INT32 bufsize, UINT32 flags);
INT32 fdt_create(void *buf, INT32 bufsize);
INT32 fdt_resize(void *fdt, void *buf, INT32 bufsize);
INT32 fdt_add_reservemap_entry(void *fdt, UINT64 addr, UINT64 size);
INT32 fdt_finish_reservemap(void *fdt);
INT32 fdt_begin_node(void *fdt, const char *name);
INT32 fdt_property(void *fdt, const char *name, const void *val, INT32 len);
INT32 fdt_property_placeholder(void *fdt, const char *name, INT32 len, void **valp);
INT32 fdt_end_node(void *fdt);
INT32 fdt_finish(void *fdt);
INT32 fdt_create_empty_tree(void *buf, INT32 bufsize);
INT32 fdt_open_into(const void *fdt, void *buf, INT32 bufsize);
INT32 fdt_pack(void *fdt);
INT32 fdt_add_mem_rsv(void *fdt, UINT64 address, UINT64 size);
INT32 fdt_del_mem_rsv(void *fdt, INT32 n);
INT32 fdt_set_name(void *fdt, INT32 nodeoffset, const char *name);
INT32 fdt_setprop(void *fdt, INT32 nodeoffset, const char *name, const void *val, INT32 len);
INT32 fdt_setprop_placeholder(void *fdt, INT32 nodeoffset, const char *name, INT32 len, void **prop_data);
INT32 fdt_appendprop(void *fdt, INT32 nodeoffset, const char *name, const void *val, INT32 len);
INT32 fdt_appendprop_addrrange(void *fdt, INT32 parent, INT32 nodeoffset, const char *name, UINT64 addr, UINT64 size);
INT32 fdt_delprop(void *fdt, INT32 nodeoffset, const char *name);
INT32 fdt_add_subnode_namelen(void *fdt, INT32 parentoffset, const char *name, INT32 namelen);
INT32 fdt_add_subnode(void *fdt, INT32 parentoffset, const char *name);
INT32 fdt_del_node(void *fdt, INT32 nodeoffset);
INT32 fdt_overlay_apply(void *fdt, void *fdto);
const char *fdt_strerror(INT32 errval);
#endif  //native end

#define AmbaFDT_Fdt32ToCpu               fdt32_to_cpu
#define AmbaFDT_Fdt64ToCpu               fdt64_to_cpu

#define AmbaFDT_OffsetPtr                fdt_offset_ptr
#define AmbaFDT_NextTag                  fdt_next_tag
#define AmbaFDT_NextNode                 fdt_next_node
#define AmbaFDT_FirstSubnode             fdt_first_subnode
#define AmbaFDT_NextSubnode              fdt_next_subnode
#define AmbaFDT_HeaderSize               fdt_header_size
#define AmbaFDT_HeaderSize_              fdt_header_size_
#define AmbaFDT_CheckHeader              fdt_check_header
#define AmbaFDT_Move                     fdt_move
#define AmbaFDT_CheckFull                fdt_check_full
#define AmbaFDT_GetString                fdt_get_string
#define AmbaFDT_String                   fdt_string
#define AmbaFDT_FindMaxPhandle           fdt_find_max_phandle
#define AmbaFDT_GeneratePhandle          fdt_generate_phandle
#define AmbaFDT_NumMemRsv                fdt_num_mem_rsv
#define AmbaFDT_GetMemRsv                fdt_get_mem_rsv
#define AmbaFDT_SubnodeOffsetNamelen     fdt_subnode_offset_namelen
#define AmbaFDT_SubnodeOffset            fdt_subnode_offset
#define AmbaFDT_PathOffsetNamelen        fdt_path_offset_namelen
#define AmbaFDT_PathOffset               fdt_path_offset
#define AmbaFDT_GetName                  fdt_get_name
#define AmbaFDT_FirstPropertyOffset      fdt_first_property_offset
#define AmbaFDT_NextPropertyOffset       fdt_next_property_offset
#define AmbaFDT_GetPropertyByOffset      fdt_get_property_by_offset
#define AmbaFDT_GetPropertyNamelen       fdt_get_property_namelen
#define AmbaFDT_GetProperty              fdt_get_property
#define AmbaFDT_GetpropByOffset          fdt_getprop_by_offset
#define AmbaFDT_GetpropNamelen           fdt_getprop_namelen
#define AmbaFDT_Getprop                  fdt_getprop
#define AmbaFDT_GetPhandle               fdt_get_phandle
#define AmbaFDT_GetAliasNamelen          fdt_get_alias_namelen
#define AmbaFDT_GetAlias                 fdt_get_alias
#define AmbaFDT_GetPath                  fdt_get_path
#define AmbaFDT_SupernodeAtdepthOffset   fdt_supernode_atdepth_offset
#define AmbaFDT_NodeDepth                fdt_node_depth
#define AmbaFDT_ParentOffset             fdt_parent_offset
#define AmbaFDT_NodeOffsetByPropValue    fdt_node_offset_by_prop_value
#define AmbaFDT_NodeOffsetByPhandle      fdt_node_offset_by_phandle
#define AmbaFDT_NodeCheckCompatible      fdt_node_check_compatible
#define AmbaFDT_NodeOffsetByCompatible   fdt_node_offset_by_compatible
#define AmbaFDT_StringlistContains       fdt_stringlist_contains
#define AmbaFDT_StringlistCount          fdt_stringlist_count
#define AmbaFDT_StringlistSearch         fdt_stringlist_search
#define AmbaFDT_StringlistGet            fdt_stringlist_get
#define AmbaFDT_AddressCells             fdt_address_cells
#define AmbaFDT_SizeCells                fdt_size_cells
//#define AmbaFDT_SetpropInplaceNamelenPartial fdt_setprop_inplace_namelen_partial
#define AmbaFDT_SetpropInplace           fdt_setprop_inplace
#define AmbaFDT_NopProperty              fdt_nop_property
#define AmbaFDT_NopNode                  fdt_nop_node
#define AmbaFDT_CreateWithFlags          fdt_create_with_flags
#define AmbaFDT_Create                   fdt_create
#define AmbaFDT_Resize                   fdt_resize
#define AmbaFDT_AddReservemapEntry       fdt_add_reservemap_entry
#define AmbaFDT_FinishReservemap         fdt_finish_reservemap
#define AmbaFDT_BeginNode                fdt_begin_node
#define AmbaFDT_PropertyPlaceholder      fdt_property_placeholder
#define AmbaFDT_EndNode                  fdt_end_node
#define AmbaFDT_Finish                   fdt_finish
#define AmbaFDT_CreateEmptyTree          fdt_create_empty_tree
#define AmbaFDT_OpenInto                 fdt_open_into
#define AmbaFDT_Pack                     fdt_pack
#define AmbaFDT_AddMemRsv                fdt_add_mem_rsv
#define AmbaFDT_DelMemRsv                fdt_del_mem_rsv
#define AmbaFDT_SetName                  fdt_set_name
#define AmbaFDT_Setprop                  fdt_setprop
#define AmbaFDT_SetpropPlaceholder       fdt_setprop_placeholder
#define AmbaFDT_Appendprop               fdt_appendprop
#define AmbaFDT_AppendpropAddrrange      fdt_appendprop_addrrange
#define AmbaFDT_Delprop                  fdt_delprop
#define AmbaFDT_AddSubnodeNamelen        fdt_add_subnode_namelen
#define AmbaFDT_AddSubnode               fdt_add_subnode
#define AmbaFDT_DelNode                  fdt_del_node
#define AmbaFDT_OverlayApply             fdt_overlay_apply
#define AmbaFDT_Strerror                 fdt_strerror


#endif /* AMBAFDT_H */
