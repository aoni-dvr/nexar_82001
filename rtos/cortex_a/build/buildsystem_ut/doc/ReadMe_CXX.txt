Libraries
==========

* libstdc++.a includes C++ STL

* Original libc.a, libgcc.a and libm.a will be used.
  For current compiling options, they are:
    - /usr/local/gcc-arm-none-eabi-7-2017-q4-major-amba-r1_20180518/arm-none-eabi/lib/v8-a/simdv8/hard/libc.a
    - /usr/local/gcc-arm-none-eabi-7-2017-q4-major-amba-r1_20180518/arm-none-eabi/lib/v8-a/simdv8/hard/libm.a
    - /usr/local/gcc-arm-none-eabi-7-2017-q4-major-amba-r1_20180518/lib/gcc/arm-none-eabi/7.2.1/v8-a/simdv8/hard/libgcc.a

* libnosys.a will be used for hooking OS APIs.
  * Customer need to implement by refering to https://sourceware.org/git/gitweb.cgi?p=newlib-cygwin.git;a=tree;f=libgloss/libnosys;hb=HEAD
  * For unit-test of building-system, it implements necessary APIs in nosys.c by using semihosting
  * Refer to refcode/project/buildsystem_ut/src/link-amba_data.sh for detail.

Constructor/Destructor
=======================

The global constructor/destructor APIs
will be placed as an array of function pointer by compiler.

Here is no loader in ThreadX,
so the global constructor APIs should be called
before running C++ applications.
An sample implementation is called AmbaCRT0() in AmbaCRT0.c

And suppose the C++ application would not be terminated.
_fini() is an example of calling all destructor APIs.

Local constructor/destructor will use new/delete,
and the memory allocation should bind to OS APIs.
AmbaCtorDtor.cpp provide sample implementation to bind to malloc()/free().
And need more large memory pool for C++ applications.


Exception tables
==================

It's not recommended to use exception,
so '-fno-exceptions -fno-rtti' is added into default CXX compiling options.
(cf. build/scripts/Makefile.lib)

The libstdc++.a still include default exception handlers.
So the exception table should be placed into .lds file
and with write permission setting of memory.

The necessary sections are surrounded by CONFIG_CC_CXX_SUPPORT
in refcode/project/buildsystem_ut/src/amba.lds.S

How to enable
==============

'make menuconfig' with:
    GCC Options  --->
        [*] Support C++ files (code size will be bigger)

