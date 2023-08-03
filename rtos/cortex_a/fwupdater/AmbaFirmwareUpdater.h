#ifndef __AMBAFIRMWARE_UPDATER_H__
#define __AMBAFIRMWARE_UPDATER_H__

typedef struct {
    UINT32  Crc32;                  /* CRC32 Checksum */
    UINT32  Version;                /* Version number */
    UINT32  Date;                   /* Date */
    UINT32  Length;                 /* Image length */
    UINT32  MemAddr;                /* Location to be loaded into memory */
    UINT32  Flag;                   /* Flag of partition    */
    UINT32  Magic;                  /* The magic number     */
    UINT32  Reserved[57];
} AMBA_FIRMWARE_IMAGE_HEADER_s;

typedef struct {
    char    ModelName[32];   /* model name */
    UINT32  PtbMagic;
    UINT32  CRC32;          /* CRC32 of entire Binary File: AmbaCamera.bin */
    struct {
        UINT32   Size;
        UINT32   Crc32;
    }  AmbaFwInfo[AMBA_NUM_USER_PARTITION];
    AMBA_PARTITION_CONFIG_s  SysPartitionInfo[AMBA_NUM_SYS_PARTITION];
    AMBA_PARTITION_CONFIG_s  UserPartitionInfo[AMBA_NUM_USER_PARTITION];
    AMBA_PLOAD_PARTITION_s   PloadInfo;
}  AMBA_FIRMWARE_HEADER_s;

struct fdt_header {
    UINT32  magic;               /* magic word FDT_MAGIC */
    UINT32  totalsize;           /* total size of DT block */
    UINT32  off_dt_struct;       /* offset to structure */
    UINT32  off_dt_strings;      /* offset to strings */
    UINT32  off_mem_rsvmap;      /* offset to memory reserve map */
    UINT32  version;             /* format version */
    UINT32  last_comp_version;   /* last compatible version */
    /* version 2 fields below */
    UINT32  boot_cpuid_phys;     /* Which physical CPU id we're booting on */
    /* version 3 fields below */
    UINT32  size_dt_strings;     /* size of the strings block */
    /* version 17 fields below */
    UINT32  size_dt_struct;      /* size of the structure block */
};

#define FDT_MAGIC   0xd00dfeed  /* 4: version, 4: total size */

/* conversion between little-endian and big-endian */
#define uswap_32(x) \
    ((((x) & 0xff000000) >> 24) | \
     (((x) & 0x00ff0000) >> 8)  | \
     (((x) & 0x0000ff00) << 8)  | \
     (((x) & 0x000000ff) << 24))

#define fdt32_to_cpu(x)       uswap_32(x)

#define fdt_get_header(fdt, field) (fdt32_to_cpu(((const struct fdt_header *)(fdt))->field))
#define fdt_magic(fdt)          (fdt_get_header(fdt, magic))
#define fdt_version(fdt)        (fdt_get_header(fdt, version))

#endif//__AMBAFIRMWARE_UPDATER_H__

