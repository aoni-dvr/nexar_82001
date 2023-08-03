/**
 *  @file AmbaVOUT_CSC.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Video Output APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

#include "AmbaVOUT.h"

static AMBA_VOUT_CSC_MATRIX_s AmbaVoutCscIdentity = {
    .Coef = {
        [0] = { 1.00000f, 0.00000f, 0.00000f },
        [1] = { 0.00000f, 1.00000f, 0.00000f },
        [2] = { 0.00000f, 0.00000f, 1.00000f },
    },
    .Offset = { [0] = 0.00000f, [1] = 0.00000f, [2] = 0.00000f },
    .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
    .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
};

static AMBA_VOUT_CSC_MATRIX_s AmbaVoutCscBt601[NUM_CSC_OPTION] = {
    /* YCbCr to RGB */
    [CSC_YCC_LIMIT_2_RGB_LIMIT] = { /* Limited to Limited */
        .Coef = {
            [0] = {  1.00000f, -0.33666f, -0.69822f },
            [1] = {  1.00000f,  1.73225f,  0.00000f },
            [2] = {  1.00000f,  0.00000f,  1.37068f },
        },
        .Offset = { [0] =  132.46f, [1] = -221.73f, [2] = -175.45f },
        .MinVal = { [0] = 16U,      [1] = 16U,      [2] = 16U      },
        .MaxVal = { [0] = 235U,     [1] = 235U,     [2] = 235U     },
    },
    [CSC_YCC_LIMIT_2_RGB_FULL] = {  /* Limited to Full */
        .Coef = {
            [0] = {  1.16438f, -0.39200f, -0.81300f },
            [1] = {  1.16438f,  2.01700f,  0.00000f },
            [2] = {  1.16438f,  0.00000f,  1.59600f },
        },
        .Offset = { [0] =  135.61f, [1] = -276.81f, [2] = -222.92f },
        .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
        .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
    },
    [CSC_YCC_FULL_2_RGB_LIMIT] = {  /* Full to Limited */
        .Coef = {
            [0] = {  0.85882f, -0.29573f, -0.61334f },
            [1] = {  0.85882f,  1.52166f,  0.00000f },
            [2] = {  0.85882f,  0.00000f,  1.20405f },
        },
        .Offset = { [0] =  132.36f, [1] = -178.77f, [2] = -138.12f },
        .MinVal = { [0] = 16U,      [1] = 16U,      [2] = 16U      },
        .MaxVal = { [0] = 235U,     [1] = 235U,     [2] = 235U     },
    },
    [CSC_YCC_FULL_2_RGB_FULL] = {   /* Full to Full */
        .Coef = {
            [0] = {  1.00000f, -0.34435f, -0.71417f },
            [1] = {  1.00000f,  1.77180f,  0.00000f },
            [2] = {  1.00000f,  0.00000f,  1.40198f },
        },
        .Offset = { [0] =  135.49f, [1] = -226.79f, [2] = -179.45f },
        .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
        .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
    },
    /* RGB to YCbCr */
    [CSC_RGB_LIMIT_2_YCC_LIMIT] = { /* Limited to Limited */
        .Coef = {
            [0] = {  0.58700f,  0.11400f,  0.29900f },
            [1] = { -0.33883f,  0.51142f, -0.17259f },
            [2] = { -0.42825f, -0.08317f,  0.51142f },
        },
        .Offset = { [0] =    0.00f, [1] =  128.00f, [2] =  128.00f },
        .MinVal = { [0] = 16U,      [1] = 16U,      [2] = 16U      },
        .MaxVal = { [0] = 235U,     [1] = 240U,     [2] = 240U     },
    },
    [CSC_RGB_LIMIT_2_YCC_FULL] = {  /* Limited to Full */
        .Coef = {
            [0] = {  0.68349f,  0.13274f,  0.34815f },
            [1] = { -0.38572f,  0.58219f, -0.19647f },
            [2] = { -0.48751f, -0.09468f,  0.58219f },
        },
        .Offset = { [0] =  -18.63f, [1] =  128.00f, [2] =  128.00f },
        .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
        .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
    },
    [CSC_RGB_FULL_2_YCC_LIMIT] = {  /* Full to Limited */
        .Coef = {
            [0] = {  0.50413f,  0.09791f,  0.25679f },
            [1] = { -0.29099f,  0.43922f, -0.14822f },
            [2] = { -0.36779f, -0.07143f,  0.43922f },
        },
        .Offset = { [0] =   16.00f, [1] =  128.00f, [2] =  128.00f },
        .MinVal = { [0] = 16U,      [1] = 16U,      [2] = 16U      },
        .MaxVal = { [0] = 235U,     [1] = 240U,     [2] = 240U     },
    },
    [CSC_RGB_FULL_2_YCC_FULL] = {   /* Full to Full */
        .Coef = {
            [0] = {  0.58700f,  0.11400f,  0.29900f },
            [1] = { -0.33126f,  0.50000f, -0.16874f },
            [2] = { -0.41869f, -0.08131f,  0.50000f },
        },
        .Offset = { [0] =    0.00f, [1] =  128.00f, [2] =  128.00f },
        .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
        .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
    },
};

static AMBA_VOUT_CSC_MATRIX_s AmbaVoutCscBt709[NUM_CSC_OPTION] = {
    /* YCbCr to RGB */
    [CSC_YCC_LIMIT_2_RGB_LIMIT] = { /* Limited to Limited */
        .Coef = {
            [0] = {  1.00000f, -0.18293f, -0.45861f },
            [1] = {  1.00000f,  1.81641f,  0.00000f },
            [2] = {  1.00000f,  0.00000f,  1.53987f },
        },
        .Offset = { [0] =   82.12f, [1] = -232.50f, [2] = -197.10f },
        .MinVal = { [0] = 16U,      [1] = 16U,      [2] = 16U      },
        .MaxVal = { [0] = 235U,     [1] = 235U,     [2] = 235U     },
    },
    [CSC_YCC_LIMIT_2_RGB_FULL] = {  /* Limited to Full */
        .Coef = {
            [0] = {  1.16438f, -0.21300f, -0.53400f },
            [1] = {  1.16438f,  2.11500f,  0.00000f },
            [2] = {  1.16438f,  0.00000f,  1.79300f },
        },
        .Offset = { [0] =   76.99f, [1] = -289.35f, [2] = -248.13f },
        .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
        .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
    },
    [CSC_YCC_FULL_2_RGB_LIMIT] = {  /* Full to Limited */
        .Coef = {
            [0] = {  0.85882f, -0.16069f, -0.40286f },
            [1] = {  0.85882f,  1.59559f,  0.00000f },
            [2] = {  0.85882f,  0.00000f,  1.35267f },
        },
        .Offset = { [0] =   88.13f, [1] = -188.24f, [2] = -157.14f },
        .MinVal = { [0] = 16U,      [1] = 16U,      [2] = 16U      },
        .MaxVal = { [0] = 235U,     [1] = 235U,     [2] = 235U     },
    },
    [CSC_YCC_FULL_2_RGB_FULL] = {   /* Full to Full */
        .Coef = {
            [0] = {  1.00000f, -0.18711f, -0.46908f },
            [1] = {  1.00000f,  1.85788f,  0.00000f },
            [2] = {  1.00000f,  0.00000f,  1.57503f },
        },
        .Offset = { [0] =   83.99f, [1] = -237.81f, [2] = -201.60f },
        .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
        .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
    },
    /* RGB to YCbCr */
    [CSC_RGB_LIMIT_2_YCC_LIMIT] = { /* Limited to Limited */
        .Coef = {
            [0] = {  0.71520f,  0.07220f,  0.21260f },
            [1] = { -0.39423f,  0.51142f, -0.11719f },
            [2] = { -0.46452f, -0.04689f,  0.51142f },
        },
        .Offset = { [0] =    0.00f, [1] =  128.00f, [2] =  128.00f },
        .MinVal = { [0] = 16U,      [1] = 16U,      [2] = 16U      },
        .MaxVal = { [0] = 235U,     [1] = 240U,     [2] = 240U     },
    },
    [CSC_RGB_LIMIT_2_YCC_FULL] = {  /* Limited to Full */
        .Coef = {
            [0] = {   0.83277f,  0.08407f,  0.24755f },
            [1] = {  -0.44879f,  0.58219f, -0.13341f },
            [2] = {  -0.52881f, -0.05338f,  0.58219f },
        },
        .Offset = { [0] =  -18.63f, [1] =  128.00f, [2] =  128.00f },
        .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
        .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
    },
    [CSC_RGB_FULL_2_YCC_LIMIT] = {  /* Full to Limited */
        .Coef = {
            [0] = {  0.61423f,  0.06201f,  0.18259f },
            [1] = { -0.33857f,  0.43922f, -0.10064f },
            [2] = { -0.39894f, -0.04027f,  0.43922f },
        },
        .Offset = { [0] =   16.00f, [1] =  128.00f, [2] =  128.00f },
        .MinVal = { [0] = 16U,      [1] = 16U,      [2] = 16U      },
        .MaxVal = { [0] = 235U,     [1] = 240U,     [2] = 240U     },
    },
    [CSC_RGB_FULL_2_YCC_FULL] = {   /* Full to Full */
        .Coef = {
            [0] = {  0.71520f,  0.07220f,  0.21260f },
            [1] = { -0.38543f,  0.50000f, -0.11457f },
            [2] = { -0.45415f, -0.04585f,  0.50000f },
        },
        .Offset = { [0] =    0.00f, [1] =  128.00f, [2] =  128.00f },
        .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
        .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
    },
};

/**
 *  Vout_GetBt601Matrix - Get color space conversion marices between RGB and YUV for BT.601
 *  @param[in] CscIndex CSC matrix index
 *  @return pointer to a CSC matrix
 */
static AMBA_VOUT_CSC_MATRIX_s * Vout_GetBt601Matrix(UINT32 CscIndex)
{
    AMBA_VOUT_CSC_MATRIX_s *pCSC = NULL;

    if (CscIndex < NUM_CSC_OPTION) {
        pCSC = &AmbaVoutCscBt601[CscIndex];
    }

    return pCSC;
}

/**
 *  Vout_GetBt709Matrix - Get color space conversion marices between RGB and YUV for BT.709
 *  @param[in] CscIndex CSC matrix index
 *  @return pointer to a CSC matrix
 */
static AMBA_VOUT_CSC_MATRIX_s * Vout_GetBt709Matrix(UINT32 CscIndex)
{
    AMBA_VOUT_CSC_MATRIX_s *pCSC = NULL;

    if (CscIndex < NUM_CSC_OPTION) {
        pCSC = &AmbaVoutCscBt709[CscIndex];
    }

    return pCSC;
}

/**
 *  Vout_GetIdentityMatrix - Get identity matrix for the original color space
 *  @return pointer to a CSC matrix
 */
static AMBA_VOUT_CSC_MATRIX_s * Vout_GetIdentityMatrix(void)
{
    return &AmbaVoutCscIdentity;
}

/**
 *  AmbaVout_GetCscMatrix - Get the specified color space conversion matrix values
 *  @param[in] CscType CSC matrix type
 *  @param[in] CscIndex CSC matrix index
 *  @param[out] pCSC pointer to a CSC matrix
 *  @return error code
 */
UINT32 AmbaVout_GetCscMatrix(UINT32 CscType, UINT32 CscIndex, AMBA_VOUT_CSC_MATRIX_s **pCSC)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    AmbaMisra_TouchUnused(&AmbaVoutCscBt601[0]);
    AmbaMisra_TouchUnused(&AmbaVoutCscBt709[0]);
    AmbaMisra_TouchUnused(&AmbaVoutCscIdentity);

    if ((CscType >= NUM_CSC_TYPE) || (pCSC == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (CscType == CSC_TYPE_BT601) {
            *pCSC = Vout_GetBt601Matrix(CscIndex);
        } else if (CscType == CSC_TYPE_BT709) {
            *pCSC = Vout_GetBt709Matrix(CscIndex);
        } else {
            *pCSC = Vout_GetIdentityMatrix();
        }

        if (*pCSC == NULL) {
            RetVal = VOUT_ERR_ARG;
        }
    }

    return RetVal;
}
