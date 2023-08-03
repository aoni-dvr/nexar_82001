#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaTypes.h"
#include "AmbaCortexA53.h"

#include "src/AmbaIPC_Vq.c"

#define AMBA_COPY     (0U)
#define AMBA_NOCOPY   (1U)
#define AMBA_SAFECOPY (2U)

struct vring_desc *gVringDesc = {0};
struct vring_used gVringUsed = {0};
struct vring_avail gVringAvail = {0};
struct AmbaIPC_VRING_s gVring = { .desc = (struct vring_desc *)&gVringDesc, .avail = &gVringAvail, .used = &gVringUsed, .num = 1U };
struct AmbaIPC_VQ_s gVq = {0};

UINT32 gBuf = 0;
UINT32 gLen = 0;

extern void Set_NoCopy_AmbaWrap_memcpy(UINT32 RetVal);

/****************************************************/
/*                init Func                         */
/****************************************************/
void init(void)
{
    gVq.vring = gVring;
}

/****************************************************/
/*                Local Stub Func                   */
/****************************************************/

/****************************************************/
/*                Main Test                         */
/****************************************************/
void TEST_vq_wait_for_completion(void)
{
    printf("TEST_vq_wait_for_completion 1\n");
    /* arrange */
    /* act */
    vq_wait_for_completion(NULL);

    printf("TEST_vq_wait_for_completion done\n");
}

void TEST_vq_complete(void)
{
    printf("TEST_vq_complete 1\n");
    /* arrange */
    /* act */
    vq_complete(NULL);

    printf("TEST_vq_complete done\n");
}

void TEST_vq_kick_prepare(void)
{
    printf("TEST_vq_kick_prepare 1\n");
    /* arrange */
    /* act */
    vq_kick_prepare(&gVq);

    printf("TEST_vq_kick_prepare done\n");
}

void TEST_vq_enable_used_notify(void)
{
    printf("TEST_vq_enable_used_notify 1\n");
    /* arrange */
    /* act */
    vq_enable_used_notify(&gVq);

    printf("TEST_vq_enable_used_notify done\n");
}

void TEST_vq_disable_used_notify(void)
{
    printf("TEST_vq_disable_used_notify 1\n");
    /* arrange */
    /* act */
    vq_disable_used_notify(&gVq);

    printf("TEST_vq_disable_used_notify done\n");
}

void TEST_vq_more_avail_buf(void)
{
    printf("TEST_vq_more_avail_buf 1\n");
    /* arrange */
    //if (vq->last_avail_idx != avail->idx)
    gVq.last_avail_idx = 0U;
    gVringAvail.idx = 1U;
    /* act */
    vq_more_avail_buf(&gVq);

    printf("TEST_vq_more_avail_buf 2\n");
    /* arrange */
    //if (vq->last_avail_idx == avail->idx)
    gVq.last_avail_idx = gVringAvail.idx = 0U;
    /* act */
    vq_more_avail_buf(&gVq);

    printf("TEST_vq_more_avail_buf done\n");
}

void TEST_vq_create(void)
{
    UINT32 vqid;

    printf("TEST_vq_create 1\n");
    /* arrange */
    // if(vqid < VQ_ALLOC_MAX)
    vqid = 0U;
    /* act */
    vq_create(NULL, NULL, 0U, NULL, 0U, vqid);

    printf("TEST_vq_create 2\n");
    /* arrange */
    // if(vqid >= VQ_ALLOC_MAX)
    vqid = VQ_ALLOC_MAX + 1U;
    /* act */
    vq_create(NULL, NULL, 0U, NULL, 0U, vqid);

    printf("TEST_vq_create done\n");
}

void TEST_vq_get_avail_buf(void)
{
    printf("TEST_vq_get_avail_buf 1\n");
    /* arrange */
    /* act */
    vq_get_avail_buf(&gVq, NULL, NULL);

    printf("TEST_vq_get_avail_buf 2\n");
    /* arrange */
    // if (vq->last_avail_idx != avail->idx)
    gVq.last_avail_idx = 0U;
    gVringAvail.idx = 1U;
    Set_NoCopy_AmbaWrap_memcpy(AMBA_NOCOPY);
    /* act */
    vq_get_avail_buf(&gVq, (void**)&gBuf, &gLen);
    Set_NoCopy_AmbaWrap_memcpy(AMBA_COPY);
    gVringAvail.idx = 0U;

    printf("TEST_vq_get_avail_buf done\n");
}

void TEST_vq_add_used_buf(void)
{
    printf("TEST_vq_add_used_buf 1\n");
    /* arrange */
    Set_NoCopy_AmbaWrap_memcpy(AMBA_NOCOPY);
    /* act */
    vq_add_used_buf(&gVq, 0U, 0U);
    Set_NoCopy_AmbaWrap_memcpy(AMBA_COPY);

    printf("TEST_vq_add_used_buf done\n");
}

void TEST_vq_more_used_buf(void)
{
    printf("TEST_vq_more_used_buf 1\n");
    /* arrange */
    // if (vq->last_used_idx != used->idx)
    gVq.last_used_idx = 0U;
    gVringUsed.idx = 1U;
    /* act */
    vq_more_used_buf(&gVq);

    printf("TEST_vq_more_used_buf 2\n");
    /* arrange */
    // if (vq->last_used_idx == used->idx)
    gVq.last_used_idx = 0U;
    gVringUsed.idx = 0U;
    /* act */
    vq_more_used_buf(&gVq);

    printf("TEST_vq_more_used_buf done\n");
}

void TEST_vq_get_used_buf(void)
{
    printf("TEST_vq_get_used_buf 1\n");
    /* arrange */
    /* act */
    vq_get_used_buf(&gVq, (void**)&gBuf, &gLen);

    printf("TEST_vq_get_used_buf 2\n");
    /* arrange */
    // if (vq->last_used_idx != used->idx)
    gVq.last_used_idx = 0U;
    gVringUsed.idx = 1U;
    Set_NoCopy_AmbaWrap_memcpy(AMBA_NOCOPY);
    /* act */
    vq_get_used_buf(&gVq, (void**)&gBuf, &gLen);
    Set_NoCopy_AmbaWrap_memcpy(AMBA_COPY);
    gVringUsed.idx = 0U;

    printf("TEST_vq_get_used_buf done\n");
}

void TEST_vq_add_avail_buf(void)
{
    printf("TEST_vq_add_avail_buf 1\n");
    /* arrange */
    Set_NoCopy_AmbaWrap_memcpy(AMBA_NOCOPY);
    /* act */
    vq_add_avail_buf(&gVq, 0U, 0U);
    Set_NoCopy_AmbaWrap_memcpy(AMBA_COPY);

    printf("TEST_vq_add_avail_buf done\n");
}

void TEST_vq_init_unused_bufs(void)
{
    printf("TEST_vq_init_unused_bufs 1\n");
    /* arrange */
    /* act */
    vq_init_unused_bufs(&gVq, &gBuf, 0U);

    printf("TEST_vq_init_unused_bufs done\n");
}

void TEST_vq_init_avail(void)
{
    printf("TEST_vq_init_avail 1\n");
    /* arrange */
    /* act */
    vq_init_avail(&gVq, 0U);

    printf("TEST_vq_init_avail done\n");
}

void TEST_vq_init_used(void)
{
    printf("TEST_vq_init_used 1\n");
    /* arrange */
    /* act */
    vq_init_used(&gVq, 0U, 0U);

    printf("TEST_vq_init_used done\n");
}

int main(void)
{
    init();

    TEST_vq_wait_for_completion();
    TEST_vq_complete();
    TEST_vq_kick_prepare();
    TEST_vq_enable_used_notify();
    TEST_vq_disable_used_notify();
    TEST_vq_more_avail_buf();
    TEST_vq_create();
    TEST_vq_get_avail_buf();
    TEST_vq_add_used_buf();
    TEST_vq_more_used_buf();
    TEST_vq_get_used_buf();
    TEST_vq_add_avail_buf();
    TEST_vq_init_unused_bufs();
    TEST_vq_init_avail();
    TEST_vq_init_used();
    return 0;
}
