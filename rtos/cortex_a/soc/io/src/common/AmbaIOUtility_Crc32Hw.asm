.global  IO_UTIL_CRC32X
.global  IO_UTIL_CRC32W
.global  IO_UTIL_CRC32H
.global  IO_UTIL_CRC32B

#ifdef CONFIG_ARM32
IO_UTIL_CRC32W:
	ldr     r3, [r0]
    crc32w  r3, r3, r1
    str     r3, [r0]
    bx      lr

IO_UTIL_CRC32H:
    ldr     r3, [r0]
    crc32h  r3, r3, r1
    str     r3, [r0]
    bx      lr

IO_UTIL_CRC32B:
    ldr     r3, [r0]
    crc32b  r3, r3, r1
    str     r3, [r0]
    bx      lr
#else

IO_UTIL_CRC32X:
	ldr     w3, [x0]
    crc32x  w3, w3, x1
    str     w3, [x0]
    ret     lr

IO_UTIL_CRC32W:
	ldr     w3, [x0]
    crc32w  w3, w3, w1
    str     w3, [x0]
    ret     lr

IO_UTIL_CRC32H:
    ldr     w3, [x0]
    crc32h  w3, w3, w1
    str     w3, [x0]
    ret     lr

IO_UTIL_CRC32B:
    ldr     w3, [x0]
    crc32b  w3, w3, w1
    str     w3, [x0]
    ret     lr

#endif
