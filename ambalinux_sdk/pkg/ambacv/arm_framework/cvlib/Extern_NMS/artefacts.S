.section .data

.align 8

.global cpu_params_start
.type cpu_params_start, %object
cpu_params_start:

.incbin CPU_PARAMS

.global cpu_params_end
.type cpu_params_end, %object
cpu_params_end:

.byte 0
