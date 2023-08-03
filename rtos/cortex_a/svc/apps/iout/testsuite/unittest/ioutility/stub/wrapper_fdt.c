#include <test_group.h>
#include <AmbaTypes.h>
#include <AmbaWrap.h>

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include <AmbaFDT.h>
#endif

UINT32 flag_fdt_check_header_fail = 0;

INT32 fdt_check_header(const void *fdt)
{
    if (flag_fdt_check_header_fail == 0U) {
        return 0;
    } else {
        return -1;
    }
}

INT32 fdt_node_offset_by_compatible(const void *fdt, INT32 startoffset, const char *compatible)
{
    if ((startoffset == 100) && (strcmp(compatible, "OffsetNegativeOne") == 0)) {
        return -1;
    }
    if ((startoffset == 100) && (strcmp(compatible, "OffsetPositiveOne") == 0)) {
        return 1;
    }
    if ((startoffset == 100) && (strcmp(compatible, "Offset455") == 0)) {
        return 455;
    }
    if ((startoffset == 100) && (strcmp(compatible, "Offset456") == 0)) {
        return 456;
    }
    return 0;
}

static struct fdt_property property_455_0 = {
    455,
    0,
    0,
    .data = {0, 0, 0, 1}
};

static struct fdt_property property_455_1 = {
    455,
    4,
    0,
    .data = {0, 0, 0, 1}
};

static UINT8 property_456_0_mem[] = {
    0xC8, 0x01, 0x00, 0x00, // tag
    0x00, 0x00, 0x00, 0x00, // length
    0x00, 0x00, 0x00, 0x00, // offset
    0x00, 0x00, 0x00, 0x01, // data[0-3]
    0x00, 0x00, 0x00, 0x02, // data[4-7]
};

static UINT8 property_456_1_mem[] = {
    0xC8, 0x01, 0x00, 0x00, // tag
    0x08, 0x00, 0x00, 0x00, // length
    0x00, 0x00, 0x00, 0x00, // offset
    0x00, 0x00, 0x00, 0x01, // data[0-3]
    0x00, 0x00, 0x00, 0x02, // data[4-7]
};

const struct fdt_property *fdt_get_property(const void *fdt, INT32 nodeoffset, const char *name, INT32 *lenp)
{
    if ((nodeoffset == 455) && (strcmp(name, "Property1") == 0)) {
        *lenp = property_455_1.len;
        return &property_455_1;
    } else if ((nodeoffset == 455) && (strcmp(name, "Property0") == 0)) {
        *lenp = property_455_0.len;
        return &property_455_0;
    } else if ((nodeoffset == 456) && (strcmp(name, "Property0") == 0)) {
        struct fdt_property *property_456_0 = (struct fdt_property *)property_456_0_mem;
        *lenp = property_456_0->len;
        return property_456_0;
    } else if ((nodeoffset == 456) && (strcmp(name, "Property1") == 0)) {
        struct fdt_property *property_456_1 = (struct fdt_property *)property_456_1_mem;
        *lenp = property_456_1->len;
        return property_456_1;
    } else {
        *lenp = 0;
        return NULL;
    }
}

INT32 fdt_node_offset_by_phandle(const void *fdt, UINT32 phandle)
{
    return (INT32)phandle;
}