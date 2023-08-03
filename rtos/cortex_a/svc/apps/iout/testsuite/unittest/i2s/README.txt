Note:
Remove AArch64 (ARM64) function attributes, a.g. general-regs-only, in driver
to run branch coverage tests on x86_64 processor.

Commands to generate report:
$ make clean
$ make
$ ./main_i2s
$ gcovr -r . -e ../stub/ --html --html-details -o i2s-html-details.html
