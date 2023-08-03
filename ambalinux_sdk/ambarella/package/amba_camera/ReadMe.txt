- Variables
    - AMBA_CAMERA_DIR="$(AMBA_CAMERA_DIR)":
        - e.g. SomePath_boss/o.cv2fs/build/amba_camera-amba
        - Would used by All making-chain
        - Binary output directory
        - Suppose use Out-Of-Source-Building (OOSB)
    - AMBARELLA_PKG_DIR=$(AMBA_CAMERA_SITE)
        - e.g. somePath_rtos/cortex_a
        - Would used by All making-chain
        - Source directory of amba_camera

- AMBA_CAMERA_DIR/mk_amba_mod.sh (automatically generated).
    - Generated during building time.
    - Used for single-build
    - amba_camera single foder
        - AMBA_CAMERA_DIR $ ./mk_amba_mod.sh vendors/ambarella/wrapper/std/lib/build/linux/
    - Linux module building
        - Building all modules
            - AMBA_CAMERA_DIR $ ./mk_amba_mod.sh amba_ko
        - Clean all modules
            - AMBA_CAMERA_DIR $ ./mk_amba_mod.sh amba_ko_clean
        - Single module building
            - AMBA_CAMERA_DIR $ ./mk_amba_mod.sh svc/build/linux/mod_hello-mk

- Modules will be built are listed at AMBA_CAMERA_DIR/lnx_mod.txt (automatically generated).

- Notes of putting source of Linux module
    - Because Linux Kbuild use relative path agaist to Makefile of module,
      it's recommended to put source under folder of Makefile.
    - If sources are out of folder, maybe symbolic-link could help.
      'rsync' with option '--copy-links' could copy the symbolic-link file as real one.
      But owner needs to take care of link-safety

