/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#ifndef AMBARELLA_DIAG_H
#define AMBARELLA_DIAG_H

#include <hw/i2c.h>

int DoI2CDiag(int argc, char *argv[]);
int DoGpioDiag(int argc, char *argv[]);
int DoTimerDiag(int argc, char *argv[]);
int DoRngDiag(int argc, char *argv[]);
int DoRtcDiag(int argc, char *argv[]);
int DoHdmiDiag(int argc, char *argv[]);
int DoAdcDiag(int argc, char *argv[]);
int DoOtpDiag(int argc, char *argv[]);
int DoSPIDiag(int argc, char *argv[]);
int DoMPrintDiag(int argc, char *argv[]);
int DoPwmDiag(int argc, char *argv[]);
int DoDmaDiag(int argc, char *argv[]);
int DoGdmaDiag(int argc, char *argv[]);
int DoCvbsDiag(int argc, char *argv[]);
int DoWdtDiag(int argc, char *argv[]);
int DoCrcDiag(int argc, char *argv[]);
int DoClockDiag(int argc, char *argv[]);
int DoNvmDiag(int argc, char *argv[]);
int DoCanDiag(int argc, char *argv[]);

#endif

