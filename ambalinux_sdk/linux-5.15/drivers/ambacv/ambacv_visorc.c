#if defined (CHIP_CV1)
#include "visorc_cv1.i"

#elif defined (CHIP_CV2S)
#include "visorc_cv22.i"

#elif defined (CHIP_CV22)
#include "visorc_cv22.i"

#elif defined (CHIP_CV25)
#include "visorc_cv25.i"

#elif defined (CHIP_CV2)
#include "visorc_cv2.i"

#elif defined (CHIP_CV2A)
#include "visorc_cv2a.i"

#elif defined (CHIP_CV22A)
#include "visorc_cv22a.i"

#elif defined (CHIP_CV2FS)
#include "visorc_cv2fs.i"

#elif defined (CHIP_CV22FS)
#include "visorc_cv22fs.i"

#elif defined (CHIP_CV28)
#include "visorc_cv28.i"

#elif defined (CHIP_CV2L)
#include "visorc_cv2l.i"

#elif defined (CHIP_CV5)
#include "visorc_cv5.i"

#elif defined (CHIP_CV52)
#include "visorc_cv52.i"

#elif defined (CHIP_CV6)
#include "visorc_cv6.i"

#else
#error "unsupported CHIP ID, please check your PROJECT setting"
#endif
