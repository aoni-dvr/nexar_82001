/**
 * @file B8Ver.c
 *
 * History:
 *    2014/12/25 - [Jenghung Luo] created file
 *
 * Copyright (C) 2004-2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include <AmbaDataType.h>
#include <AmbaVer.h>

void AmbaVer_SetB8Ver(AMBA_VerInfo_s *pVerInfo)
{
    /* pVerInfo->Key is initialized in other place, pls keep it. */

    pVerInfo->MachStr   = AMBA_BUILD_MACHINE;
    pVerInfo->DateStr   = AMBA_BUILD_DATE;
    pVerInfo->DateNum   = AMBA_BUILD_DATE_NUM;
    pVerInfo->SecNum    = AMBA_BUILD_SEC_NUM;
    pVerInfo->CiCount   = AMBA_CI_COUNT;
    pVerInfo->CiIdStr   = AMBA_CI_ID;
}
