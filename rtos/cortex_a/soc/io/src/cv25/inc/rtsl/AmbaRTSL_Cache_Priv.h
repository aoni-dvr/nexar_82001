/**
 *  @file AmbaRTSL_Cache_Priv.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Definitions & Constants for Chip-Dependent Cache Related APIs
 *
 */

#ifndef AMBA_RTSL_CACHE_PRIV_H
#define AMBA_RTSL_CACHE_PRIV_H

/*
 *  Cortex-A53:
 *      L1 Instruction Cache Size 8/16/32/64 KB (per core), 2 ways (4/8/16/32 KB per way), 64 bytes per line. (64/128/256/512 sets)
 *      L1 Data Cache Size 8/16/32/64 KB (per core), 4 ways (2/4/8/16 KB per way), 64 bytes per line. (32/64/128/256 sets)
 *      L2 Data Cache Size 128/256/512/1024/2048 KB, 16 ways (8/16/32/64/128 KB per way), 64 bytes per line. (128/256/512/1024/2048 sets)
 */
/*
 *  CV25:
 *      L1 Instruction Cache Size 32 KB (per core), 2 ways (16 KB per way), 64 bytes per line. (16384/64=256 sets)
 *      L1 Data Cache Size 32 KB (per core), 4 ways (8 KB per way), 64 bytes per line. (8192/64=128 sets)
 *      L2 Data Cache Size 1024 KB, 16 ways (64 KB per way), 64 bytes per line. (65536/64=1024 sets)
 */

/* Cache level 1 */
static inline void CACHE_DataCacheCleanL1(void)
{
    UINT32 Set, Way;
    UINT32 RegData;

    /* clean cache lines in all ways */
    for (Way = 0U; Way < 4U; Way++) {
        /* Way, bits[31:30], the number of the way to operate on. */
        RegData = (Way << 30U); /* | ((UINT32)0U << 1U); */
        for (Set = 0U; Set < 128U; Set++) {
            /* Set, bits[12:6], the number of the set to operate on. */
#ifdef CONFIG_ARM64
            AmbaCache_DataCleanSetWay(RegData | (Set << 6U));
#else
            AmbaCache_DataCleanSetWayA32(RegData | (Set << 6U));
#endif
        }
    }

    /* Ensure completion of the clean */
    AMBA_DSB();
}

static inline void CACHE_DataCacheCleanInvldL1(void)
{
    UINT32 Set, Way;
    UINT32 RegData;

    /* clean and invalidate cache lines in all ways */
    for (Way = 0U; Way < 4U; Way++) {
        /* Way, bits[31:30], the number of the way to operate on. */
        RegData = (Way << 30U); /* | (0U << 1U); */
        for (Set = 0U; Set < 128U; Set++) {
            /* Set, bits[12:6], the number of the set to operate on. */
#ifdef CONFIG_ARM64
            AmbaCache_DataCleanInvSetWay(RegData | (Set << 6U));
#else
            AmbaCache_DataCleanInvSetWayA32(RegData | (Set << 6U));
#endif
        }
    }

    /* Ensure completion of the clean&inval (no data lost) */
    AMBA_DSB();
}

static inline void CACHE_DataCacheInvldL1(void)
{
    UINT32 Set, Way;
    UINT32 RegData;

    /* invalidate cache lines in all ways */
    for (Way = 0U; Way < 4U; Way++) {
        /* Way, bits[31:30], the number of the way to operate on. */
        RegData = (Way << 30U); /* | ((UINT32)0U << 1U); */
        for (Set = 0U; Set < 128U; Set++) {
            /* Set, bits[12:6], the number of the set to operate on. */
#ifdef CONFIG_ARM64
            AmbaCache_DataInvSetWay(RegData | (Set << 6U));
#else
            AmbaCache_DataInvSetWayA32(RegData | (Set << 6U));
#endif
        }
    }

    /* Ensure completion of the inval */
    AMBA_DSB();
}

/* Cache level 2 */
static inline void CACHE_DataCacheCleanL2(void)
{
    UINT32 Set, Way;
    UINT32 RegData;

    /* clean cache lines in all ways */
    for (Way = 0U; Way < 16U; Way++) {
        /* Way, bits[31:28], the number of the way to operate on. */
        RegData = (Way << 28U) | ((UINT32)1U << 1U);
        for (Set = 0U; Set < 1024U; Set++) {
            /* Set, bits[15:6], the number of the set to operate on. */
#ifdef CONFIG_ARM64
            AmbaCache_DataCleanSetWay(RegData | (Set << 6U));
#else
            AmbaCache_DataCleanSetWayA32(RegData | (Set << 6U));
#endif
        }
    }

    /* Ensure completion of the clean */
    AMBA_DSB();
}

static inline void CACHE_DataCacheCleanInvldL2(void)
{
    UINT32 Set, Way;
    UINT32 RegData;

    /* clean and invalidate cache lines in all ways */
    for (Way = 0U; Way < 16U; Way++) {
        /* Way, bits[31:28], the number of the way to operate on. */
        RegData = (Way << 28U) | ((UINT32)1U << 1U);
        for (Set = 0U; Set < 1024U; Set++) {
            /* Set, bits[15:6], the number of the set to operate on. */
#ifdef CONFIG_ARM64
            AmbaCache_DataCleanInvSetWay(RegData | (Set << 6U));
#else
            AmbaCache_DataCleanInvSetWayA32(RegData | (Set << 6U));
#endif
        }
    }

    /* Ensure completion of the clean&inval (no data lost) */
    AMBA_DSB();
}

static inline void CACHE_DataCacheInvldL2(void)
{
    UINT32 Set, Way;
    UINT32 RegData;

    /* invalidate cache lines in all ways */
    for (Way = 0U; Way < 16U; Way++) {
        /* Way, bits[31:28], the number of the way to operate on. */
        RegData = (Way << 28U) | ((UINT32)1U << 1U);
        for (Set = 0U; Set < 1024U; Set++) {
            /* Set, bits[15:6], the number of the set to operate on. */
#ifdef CONFIG_ARM64
            AmbaCache_DataInvSetWay(RegData | (Set << 6U));
#else
            AmbaCache_DataInvSetWayA32(RegData | (Set << 6U));
#endif
        }
    }

    /* Ensure completion of the inval */
    AMBA_DSB();
}

#endif  /* AMBA_RTSL_CACHE_PRIV_H */
