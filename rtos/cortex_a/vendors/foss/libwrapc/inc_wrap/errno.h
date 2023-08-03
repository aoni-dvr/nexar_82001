#ifdef CONFIG_CC_USESTD
#error "This is pseudo header when -nostdinc"
#else

#ifndef AMBA_STDWRAP_ERRNO_H
#define AMBA_STDWRAP_ERRNO_H

#define ENOENT 2        /* No such file or directory */

#define ENOMEM 12       /* Not enough space */

#define EBUSY  16       /* Device or resource busy */
#define EEXIST 17       /* File exists */

#define ENODEV 19       /* No such device */

#define EINVAL 22       /* Invalid argument */

#define ERANGE 34       /* Result too large */

#endif // AMBA_STDWRAP_ERRNO_H

#endif // CONFIG_CC_USESTD

