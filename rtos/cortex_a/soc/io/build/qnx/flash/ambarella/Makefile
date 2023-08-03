EXTRA_SRCVPATH += $(CURDIR)/../../../../src/common/spinor/qnx/flash/boards/qspi-amba/
EXTRA_SRCVPATH += $(CURDIR)/../../../../src/common/spinor/qnx/flash/boards/qspi-amba/ambarella/

# src
SRCS := f3s_qspi_close.o
SRCS += f3s_qspi_erase.c
SRCS += f3s_qspi_ident.c
SRCS += f3s_qspi_main.c
SRCS += f3s_qspi_open.c
SRCS += f3s_qspi_page.c
SRCS += f3s_qspi_read.c
SRCS += f3s_qspi_reset.c
SRCS += f3s_qspi_status.c
SRCS += f3s_qspi_sync.c
SRCS += f3s_qspi_write.c

#ambarella
SRCS += amba_qspi.c
SRCS += qspi_cmds.c

LIST=CPU
include recurse.mk
