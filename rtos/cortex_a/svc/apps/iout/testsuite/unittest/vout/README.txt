Commands to generate report:
$ make clean
$ make
$ ./main_vout
$ gcovr -r . -e ../stub/ -f . --html --html-details -o vout-html-details.html
