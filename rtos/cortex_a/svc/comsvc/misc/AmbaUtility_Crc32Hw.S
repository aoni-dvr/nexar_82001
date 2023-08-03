.global  CRC32X
.global  CRC32W
.global  CRC32H
.global  CRC32B

#if defined(CONFIG_QNX) || defined(CONFIG_ARM64)

CRC32X:
	ldr     w3, [x0]
    crc32x  w3, w3, x1
    str     w3, [x0]
    ret     x30

CRC32W:
	ldr     w3, [x0]
    crc32w  w3, w3, w1
    str     w3, [x0]
    ret     x30

CRC32H:
    ldr     w3, [x0]
    crc32h  w3, w3, w1
    str     w3, [x0]
    ret     x30

CRC32B:
    ldr     w3, [x0]
    crc32b  w3, w3, w1
    str     w3, [x0]
    ret     x30

#else

CRC32W:
    ldr     r3, [r0]
    crc32w  r3, r3, r1
    str     r3, [r0]
    bx      lr

CRC32H:
    ldr     r3, [r0]
    crc32h  r3, r3, r1
    str     r3, [r0]
    bx      lr

CRC32B:
    ldr     r3, [r0]
    crc32b  r3, r3, r1
    str     r3, [r0]
    bx      lr

#endif
