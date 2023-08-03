#ifndef  AMBAMAL_IOCTL_H
#define  AMBAMAL_IOCTL_H

#if !defined(CONFIG_BUILD_CV_THREADX)
#define AMBAMAL_DEV "/dev/ambamal"

#define AMBA_MAL_KERNEL_VERSION       1U

typedef struct {
    UINT32 Version;
    UINT32 Number;
} AMBA_MAL_IOCTL_VERSION_s;

typedef struct {
    AMBA_MAL_INFO_s Info;
    char Name[64];
} AMBA_MAL_IOCTL_INFO_s;

typedef struct {
    UINT64 PhysAddr;
    UINT64 GlobalAddr;
    UINT64 Size;
    UINT32 Reserved[2];
} AMBA_MAL_SEG_INFO_s;

#define AMBA_MAL_SEG_NUM (16U)

typedef struct {
    UINT32 SegNum;
    AMBA_MAL_SEG_INFO_s SegList[AMBA_MAL_SEG_NUM];
} AMBA_MAL_IOCTL_SEG_INFO_s;

#define AMBA_MAL_GET_VERSION       _IOR  ('A', 0x0U, AMBA_MAL_IOCTL_VERSION_s)
#define AMBA_MAL_GET_INFO          _IOWR ('A', 0x1U, AMBA_MAL_IOCTL_INFO_s)
#define AMBA_MAL_GET_SEG_INFO      _IOWR ('A', 0x2U, AMBA_MAL_IOCTL_SEG_INFO_s)

#endif

#endif  //AMBAMAL_IOCTL_H
