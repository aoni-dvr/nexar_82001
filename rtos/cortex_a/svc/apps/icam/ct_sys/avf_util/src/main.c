/**
*  @file main.c
*
* Copyright (c) [2020] Ambarella International LP
*
* This file and its contents ("Software") are protected by intellectual
* property rights including, without limitation, U.S. and/or foreign
* copyrights. This Software is also the confidential and proprietary
* information of Ambarella International LP and its licensors. You may not use, reproduce,
* disclose, distribute, modify, or otherwise prepare derivative works of this
* Software or any portion thereof except pursuant to a signed license agreement
* or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
* In the absence of such an agreement, you agree to promptly notify and return
* this Software to Ambarella International LP.
*
* This file includes sample code and is only for internal testing and evaluation.  If you
* distribute this sample code (whether in source, object, or binary code form), it will be
* without any warranty or indemnity protection from Ambarella International LP or its affiliates.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
* MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*  @detail xxxx
*
*/
#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include "AmbaSYS.h"
#include "avf_util.h"

/*---------------------------------------------------------------------------*\
 * Command line options
\*---------------------------------------------------------------------------*/
static struct option long_options[] =
{
  {"clockinfo",      0, NULL, 'c'},
  {"reboot",         0, NULL, 'r'},
  {0, 0, 0, 0},
};

static const char *short_options = "cr";

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avfu_parse_opt
 *
 *  @Description:: Parse the arguments
 *
 *  @Input      ::
 *      argc:      Argument count
 *      argv:      Argument array
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *      int        OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t avfu_parse_opt(int32_t argc, char **argv)
{
  int32_t ret = AVFU_OK;
  int32_t c;

  optind = 1;
  while ((c = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
    switch (c) {
      case 'c':
        {
          uint32_t freq = 0U;
          printf("\r\nClock Information:\r\n");
          (void)AmbaSYS_GetClkFreq(AMBA_SYS_CLK_CORTEX, &freq);
          printf("        gclk_cortex:    %u Hz\r\n", freq);
          (void)AmbaSYS_GetClkFreq(AMBA_SYS_CLK_DRAM, &freq);
          printf("        gclk_ddr:       %u Hz\r\n", freq);
          (void)AmbaSYS_GetClkFreq(AMBA_SYS_CLK_CORE, &freq);
          printf("        gclk_core:      %u Hz\r\n", freq);
          (void)AmbaSYS_GetClkFreq(AMBA_SYS_CLK_VISION, &freq);
          printf("        gclk_vision:    %u Hz\r\n", freq);
        }
        break;
      case 'r':
        printf("Reboot!!\r\n");
        (void)AmbaSYS_Reboot();
        break;
      default:
        printf("NG!!\r\n");
        ret = AVFU_NG;
    }
  }

  return ret;
}

int32_t main(int32_t argc, char **argv)
{
  int32_t ret = AVFU_OK;

  ret = avfu_parse_opt(argc, argv);

  return ret;
}

