#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "aipc_user.h"
#include "AmbaIPC_RpcProg_RT_Bbx.h"

#include "bbx_handler.h"

#define DEV_MEM "/dev/ppm"

typedef struct ambaipc_clnt_s {
    int host;
    int prog;
    int ver;
    int timeout;
} ambaipc_clnt_t;

#define DEBUG(...) //printf(...)

static ambaipc_clnt_t bbxhandler_prog =
{
    RT_BBX_HOST,
    RT_BBX_PROG_ID,
    RT_BBX_VER,
    RT_BBX_DEFULT_TIMEOUT
};

static CLIENT_ID_t clnt_bbxhandler = 0;

static void *do_mmap(unsigned long long phy_addr, unsigned long long size, void **mmap_base, unsigned long long *mmap_size)
{
    unsigned char *map_base;
    unsigned long long addr_4k, size_4k;
    int fd;

    *mmap_size = 0;
    DEBUG("%s: phy_addr=0x%llx size=0x%llx\n", __FUNCTION__, phy_addr, size);

    addr_4k = (phy_addr >> 12) << 12;
    size_4k = size + (phy_addr - addr_4k);

    DEBUG("%s: addr_4k=0x%llx  size_4k=0x%llx\n", __FUNCTION__, addr_4k, size_4k);

    fd = open(DEV_MEM, O_RDWR | O_SYNC);
    if (fd == -1) {
        return NULL;
    }

    map_base = mmap(NULL, size_4k, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr_4k);
    if (map_base == MAP_FAILED) {
        perror("Fail to do mmap:\n");
        fprintf(stderr, "phy_addr=0x%llx size=0x%llx\n",phy_addr, size);
        fprintf(stderr, "addr_4k=0x%llx  size_4k=0x%llx\n", addr_4k, size_4k);
        return NULL;
    } else {
        DEBUG("Successfull! mmap 0x%llx to %p.\n", addr_4k, map_base);
        *mmap_base = map_base;
        *mmap_size = size_4k;
        map_base += (phy_addr - addr_4k);
        DEBUG("mmap_base=%p, mmap_size=%llu, map_base=%p\n",*mmap_base,*mmap_size,map_base);
    }

    return (void *)map_base;
}

static int do_unmap(void *mmap_base, unsigned long long size)
{
    return munmap(mmap_base, size);
}

static int CheckClientId(void)
{
    int status = 0;

    if (clnt_bbxhandler == 0) {
        clnt_bbxhandler = ambaipc_clnt_create(bbxhandler_prog.host,
                                              bbxhandler_prog.prog,
                                              bbxhandler_prog.ver);
        if (clnt_bbxhandler==0) {
            fprintf(stderr,"%s: Fail to create client.\n",__FUNCTION__);
            status = -1;
        } else {
            status = 0;
        }
    }

    return status;
}

int Bbx_Create_Buffer(Bbx_Buffer_Setting_s *Setting, Bbx_Buffer_Info_s *Buffer)
{
    int status = 0;
    int rval = -1;
    RT_Bbx_Buffer_Setting_s BufSetting = {0};
    RT_Bbx_Buffer_Info_s Result = {0};

    //printf("%s called\n",__FUNCTION__);

    if ((Setting == NULL) || (Buffer == NULL)) {
        fprintf(stderr, "%s: invalid parameters. Setting=%p, Buffer=%p\n", __FUNCTION__, Setting, Buffer);
        return -1;
    }

    status = CheckClientId();
    if (status == 0) {
        BufSetting.MemType = Setting->MemType;
        BufSetting.Size = Setting->Size;
        BufSetting.Align = Setting->Align;

        status = ambaipc_clnt_call(clnt_bbxhandler, RT_BBX_FUNC_CREATEBUF,
                &BufSetting, sizeof(RT_Bbx_Buffer_Setting_s),
                &Result, sizeof(RT_Bbx_Buffer_Info_s),
                bbxhandler_prog.timeout);
        if (status != 0) {
            fprintf(stderr,"%s: fail to do ambaipc_clnt_call(). %d\n", __FUNCTION__, status);
            rval = -1;
        } else {
            Buffer->RawAddr = (void *)(Result.RawAddr);
            Buffer->AlignedAddr = (void *)(Result.AlignedAddr);
            Buffer->MmapedAddr = do_mmap(Result.AlignedAddr, BufSetting.Size, &(Buffer->MmapedBase), &(Buffer->MmapedSize));
            if (Buffer->MmapedAddr == NULL) {
                fprintf(stderr,"%s: fail to do do_mmap().\n", __FUNCTION__);
                rval = -1;
                Bbx_Free_Buffer(Buffer);
            } else {
                rval = 0;
            }
        }
    } else {
        fprintf(stderr,"%s: Fail to create client.\n",__FUNCTION__);
    }

    return rval;
}

int Bbx_Free_Buffer(Bbx_Buffer_Info_s *Buffer)
{
    int status = 0;
    int rval = -1;
    RT_Bbx_Buffer_Info_s BufInfo = {0};

    //printf("%s called\n",__FUNCTION__);

    if (Buffer == NULL) {
        fprintf(stderr, "%s: invalid parameters. Buffer=%p\n", __FUNCTION__, Buffer);
        return -1;
    }

    if (Buffer->MmapedSize != 0) {
        rval = do_unmap(Buffer->MmapedBase, Buffer->MmapedSize);
        if (rval != 0) {
            fprintf(stderr, "%s: Fail to do_unmap.\n", __FUNCTION__);
            return -1;
        } else {
            Buffer->MmapedAddr = NULL;
            Buffer->MmapedBase = NULL;
            Buffer->MmapedSize = 0;
        }
    }

    status = CheckClientId();
    if (status == 0) {
        BufInfo.RawAddr = (unsigned long long) Buffer->RawAddr;
        BufInfo.AlignedAddr = (unsigned long long) Buffer->AlignedAddr;

        status = ambaipc_clnt_call(clnt_bbxhandler, RT_BBX_FUNC_FREEBUF,
                &BufInfo, sizeof(RT_Bbx_Buffer_Info_s),
                &rval, sizeof(int),
                bbxhandler_prog.timeout);
        if (status != 0) {
            fprintf(stderr,"%s: fail to do ambaipc_clnt_call(). %d\n", __FUNCTION__, status);
            rval = -1;
        } else {
            Buffer->RawAddr = NULL;
            Buffer->AlignedAddr = NULL;
        }
    } else {
        fprintf(stderr,"%s: Fail to create client.\n",__FUNCTION__);
    }

    return rval;
}

int Bbx_SetLogHeader(Bbx_Header_s *Header)
{
    int status;
    int rval = -1, i;
    RT_Bbx_Header_s BbxHeader = {0};

    //printf("%s called\n",__FUNCTION__);

    if (Header == NULL) {
        fprintf(stderr, "%s: invalid parameters. Header=%p\n", __FUNCTION__, Header);
        return -1;
    }

    status = CheckClientId();
    if (status == 0) {
        BbxHeader.Ver = Header->Ver;
        for (i=0; i<BBX_MAX_ROI_AMOUNT; i++) {
            BbxHeader.Roi[i][0] = Header->Roi[i][0];
            BbxHeader.Roi[i][1] = Header->Roi[i][1];
            BbxHeader.Roi[i][2] = Header->Roi[i][2];
            BbxHeader.Roi[i][3] = Header->Roi[i][3];
        }
        status = ambaipc_clnt_call(clnt_bbxhandler, RT_BBX_FUNC_SETHEADER,
                &BbxHeader, sizeof(RT_Bbx_Header_s),
                &rval, sizeof(int),
                bbxhandler_prog.timeout);
        if (status != 0) {
            fprintf(stderr,"%s: fail to do ambaipc_clnt_call(). %d\n", __FUNCTION__, status);
            rval = -1;
        }
    } else {
        fprintf(stderr,"%s: Fail to create client.\n",__FUNCTION__);
    }

    return rval;
}

int Bbx_Update(Bbx_Info_s *Info)
{
    int status = 0;
    int rval = -1;
    RT_Bbx_Info_s BbxInfo = {0};

    //printf("%s called\n",__FUNCTION__);

    if (Info == NULL) {
        fprintf(stderr, "%s: invalid parameters. Info=%p\n", __FUNCTION__, Info);
        return -1;
    }

    status = CheckClientId();
    if (status == 0) {
        BbxInfo.Flags = Info->Flags;
        BbxInfo.Size = Info->Size;
        BbxInfo.Addr = (unsigned long long)(Info->Addr);

        status = ambaipc_clnt_call(clnt_bbxhandler, RT_BBX_FUNC_UPDATE,
                &BbxInfo, sizeof(RT_Bbx_Info_s),
                &rval, sizeof(int),
                bbxhandler_prog.timeout);
        if (status != 0) {
            fprintf(stderr,"%s: fail to do ambaipc_clnt_call(). %d\n", __FUNCTION__, status);
            rval = -1;
        }
    } else {
        fprintf(stderr,"%s: Fail to create client.\n",__FUNCTION__);
    }

    return rval;
}

int Bbx_Init(void)
{
    int rval = -1;

    rval = CheckClientId();
    if (rval != 0) {
        fprintf(stderr,"%s: Fail to create client.\n",__FUNCTION__);
    }

    return rval;
}

int Bbx_Release(void)
{
    int rval = 0;

    if (clnt_bbxhandler != 0){
        rval = ambaipc_clnt_destroy(clnt_bbxhandler);
        if(rval == 0){
            clnt_bbxhandler = 0;
        }
    }

    return rval;
}

