* Setup toolchain for Kbuild, run:
    source build/maintenance/env/env_set.sh [cv2|cv22|...]

* Setup environment for QNX, run:
    source /opt/qnx700/qnxsdp-env.sh

* Use default configuration, run:
    make \
        KBUILD_KCONFIG=svc/build/qnx/Kconfig.Qnx \
        KBUILD_AMBA_MKFILE=svc/build/qnx/Makefile.Amba \
        ../svc/build/qnx/xxx_defconfig

* Compiling, run:
    make \
        KBUILD_KCONFIG=svc/build/qnx/Kconfig.Qnx \
        KBUILD_AMBA_MKFILE=svc/build/qnx/Makefile.Amba \
        Your-targets

* Your-targets could be
    * n/a: build all
    * clean: clean generated files.
    * Path-mk: single build
        * 'Path' should be the one noted at *mod_dir in svc/build/qnx/qnx.mk
        * e.g. vendors/ambarella/wrapper/std/lib/build/qnx
    * amba_fwprog: Images for firmware programmer.
    * distribute: Used by QA

