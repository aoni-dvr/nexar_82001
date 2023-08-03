/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCortexEMMC.asm
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: NAND Parameters for ARM11
 *
 *  @History        ::
 *      Date        Name        Comments
 *
 *-------------------------------------------------------------------------------------------------------------------*/


        .global  AmbaWarmBootMagicCode
        .global  AmbaNandTiming
        .global  EmmcBusWidth

        .section BstEmmcParam

        .align 8
AmbaWarmBootMagicCode:
        .string  "AmbarellaCamera"

EmmcBusWidth:
        .word  0x8
