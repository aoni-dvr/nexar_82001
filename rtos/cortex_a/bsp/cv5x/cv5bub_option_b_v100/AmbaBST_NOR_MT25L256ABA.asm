/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaBST_NOR_MT25Q256ABA.asm
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: NOR Parameters
\*-------------------------------------------------------------------------------------------------------------------*/

        .global AmbaWarmBootMagicCode
        .global BytePerBlock
        .global BytePerPage
        .global BstPageCount
        .global CmdParameter
        .global DummyLength

        .data
        .section BstNorParam

        .align 8
AmbaWarmBootMagicCode:
        .string  "AmbarellaCamera"

BytePerBlock:
        .word    0x10000

BytePerPage:
        .word    0x100

BstPageCount:
        .word    0x10

CmdParameter:
        .word    0xBBF82400

DummyLength:
        .word    0x5A000000
