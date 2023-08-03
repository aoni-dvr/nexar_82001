/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSpiNAND_Generic.asm
 *
 *  @Copyright      :: Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Generic NAND Parameters for Bootstrap
\*-------------------------------------------------------------------------------------------------------------------*/

        .global AmbaWarmBootMagicCode
        .global AmbaSpiNandTiming
        .global AmbaPlaneSelect

        .data
        .section BstSpiNandParam

AmbaWarmBootMagicCode:
        .string "AmbarellaCamera"

AmbaSpiNandTiming:
        .word   0x14141414
        .word   0x3030303
        .word   0x80b36

AmbaPlaneSelect:
        /* Choose bit6 of row address as plane select */
        .word   0x20000000
