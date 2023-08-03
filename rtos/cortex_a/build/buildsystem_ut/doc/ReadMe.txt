* Ensure toolchain path is in your PATH environment.

* Config, run:
    make KBUILD_KCONFIG=build/buildsystem_ut/kconfig/Kconfig KBUILD_AMBA_MKFILE=build/buildsystem_ut/Makefile.Amba ../build/buildsystem_ut/configs/ca53_neon_defconfig

* Compiling, run:
    make KBUILD_KCONFIG=build/buildsystem_ut/kconfig/Kconfig KBUILD_AMBA_MKFILE=build/buildsystem_ut/Makefile.Amba


* To make typing easy, you can use shell script or alias to shorten commands,
    * by alias: alias mk='make KBUILD_KCONFIG=build/buildsystem_ut/kconfig/Kconfig KBUILD_AMBA_MKFILE=build/buildsystem_ut/Makefile.Amba'
    * by shell script:
        #!/bin/bash
        make KBUILD_KCONFIG=build/buildsystem_ut/kconfig/Kconfig KBUILD_AMBA_MKFILE=build/buildsystem_ut/Makefile.Amba $*


* Testing with QEMU, make sure that here is qemu installed, run:
    make KBUILD_KCONFIG=build/buildsystem_ut/kconfig/Kconfig KBUILD_AMBA_MKFILE=build/buildsystem_ut/Makefile.Amba amba_test
  * It will use qemu-system-aarch64 to run the test-cases.
  * The qemu-system-aarch64 is patched to run AArch32 only::
        diff --git a/target/arm/cpu64.c b/target/arm/cpu64.c
        index d0581d5..e27761e 100644
        --- a/target/arm/cpu64.c
        +++ b/target/arm/cpu64.c
        @@ -167,7 +167,9 @@ static void aarch64_a53_initfn(Object *obj)
            set_feature(&cpu->env, ARM_FEATURE_VFP4);
            set_feature(&cpu->env, ARM_FEATURE_NEON);
            set_feature(&cpu->env, ARM_FEATURE_GENERIC_TIMER);
        +#if 0
            set_feature(&cpu->env, ARM_FEATURE_AARCH64);
        +#endif
            set_feature(&cpu->env, ARM_FEATURE_CBAR_RO);
            set_feature(&cpu->env, ARM_FEATURE_V8_AES);
            set_feature(&cpu->env, ARM_FEATURE_V8_SHA1);

