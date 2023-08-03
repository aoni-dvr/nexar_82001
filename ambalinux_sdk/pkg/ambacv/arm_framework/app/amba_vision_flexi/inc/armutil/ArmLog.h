/**
*  @file ArmLog.h
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
*   @details The arm print log functions
*
*/

#ifndef ARM_LOG_H
#define ARM_LOG_H

#include <stdio.h>

//Regular bold text
#define RED         "1;31"
#define GREEN       "1;32"
#define YELLOW      "1;33"

#define ArmLog_OK(Module,fmt,...)      printf("\033[%sm[%s|OK] "fmt"\033[m\n",YELLOW,Module,##__VA_ARGS__)
#define ArmLog_ERR(Module,fmt,...)      printf("\033[%sm[%s|ERR] "fmt"\033[m\n",RED,Module,##__VA_ARGS__)
#define ArmLog_WARN(Module,fmt,...)      printf("\033[%sm[%s|WARN] "fmt"\033[m\n",GREEN,Module,##__VA_ARGS__)
#define ArmLog_DBG(Module,fmt,...)      printf("[%s|DBG] "fmt"\n",Module,##__VA_ARGS__)
#define ArmLog_STR(Module,fmt,...)      printf("[%s|DBG] "fmt"\n",Module,##__VA_ARGS__)

#endif  /* ARM_LOG_H */
